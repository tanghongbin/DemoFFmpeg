//
// Created by Admin on 2021/11/22.
//

#include <cstring>
#include "../../../play_header/decoder/hw/BaseHwDecoder.h"
#include <string>
#include <render/VideoFboRender.h>
#include <render/VideoSpecialEffortsRender.h>
//#include <StringUtils>

// 2-视频向音频同步
#define SYNC_TYPE 1

BaseHwDecoder::BaseHwDecoder(){
    decodeThread = 0;
    audioRender = 0;
    videoRender = 0;
    isInit = false;
    mMediaCodec = 0;
    mMediaExtractor = 0;
    timeSyncHelper = 0;
    i420dst = 0;
    outputDataListener = 0;
    readyCall = 0;
    isNeedPauseWhenFinished = true;
    isNeedRender = true;
    onCompleteCall = 0;
    videoConfigInfo = 0;
}

int64_t BaseHwDecoder::getCurrentAudioPtsUs(){
    return currentPtsUs;
}

void BaseHwDecoder::Init(const char * url) {
    if (isInit) return;
    LOGCATE("打印地址:%s",url);
    seekProgress = -1;
    isFinished = false;
    isPaused = true;
    mVideoWidth = mVideoHeight = mWindowWidth = mWindowHeight = oreration = 0;
    const char * testUrl = "http://freesaasofficedata.oss.aliyuncs.com/2222/20190930142639768.mp4";
    strcpy(mUrl,url);
    timeSyncHelper = new TimeSyncHelper;
    mMediaExtractor = AMediaExtractor_new();
    media_status_t status;
    if (startWith(mUrl,"http") || startWith(mUrl,"https")){
        status = AMediaExtractor_setDataSource(mMediaExtractor, mUrl);
        LOGCATE("设置网络地址:%s",mUrl);
    } else {
        int64_t fileSize = getFileSize(mUrl);
        int64_t offset = 0;
        FILE *avFile = fopen(mUrl, "rb+");
        int avFd = fileno(avFile);
        status = AMediaExtractor_setDataSourceFd(mMediaExtractor,avFd,offset,fileSize);
        fclose(avFile);
        LOGCATE("打印资源文件大小:%lld",fileSize);
    }
    if (status != AMEDIA_OK) {
        MsgLoopHelper::sendMsg(Message::obtain(JNI_COMMUNICATE_TYPE_ERROR,0,0,"设置资源错误"));
        return;
    }
    size_t trackCount = AMediaExtractor_getTrackCount(mMediaExtractor);
    for (int i = 0; i < trackCount; ++i) {
        AMediaFormat* trackFormat = AMediaExtractor_getTrackFormat(mMediaExtractor,i);
        const char* mimeStr;
        AMediaFormat_getString(trackFormat,AMEDIAFORMAT_KEY_MIME,&mimeStr);
        std::string result  = mimeStr;
        if (startWith(result.c_str(),getDecodeTypeStr())) {
            mMediaCodec = AMediaCodec_createDecoderByType(mimeStr);
            AMediaExtractor_selectTrack(mMediaExtractor,i);
            LOGCATE("本次选择结果:%s",AMediaFormat_toString(trackFormat));
            media_status_t configResult = AMediaCodec_configure(mMediaCodec, trackFormat, nullptr,
                                                                nullptr, 0);
            if (configResult != AMEDIA_OK) {
                LOGCATE("config error:%d",configResult);
                return;
            }
            AMediaCodec_start(mMediaCodec);
        }
        // 视音频发送时长
        if (appointMediaType == 1 && startWith(result.c_str(),"audio")) {
            int64_t duration64;
            AMediaFormat_getInt64(trackFormat,AMEDIAFORMAT_KEY_DURATION,&duration64);
            int durationInt = duration64 / 1000 / 1000;
            MsgLoopHelper::sendMsg(Message::obtain(JNI_COMMUNICATE_TYPE_DURATION,0,durationInt));
            LOGCATE("打印视频时长：%d  int64:%lld", durationInt,duration64);
        } else if (appointMediaType == 2 && startWith(result.c_str(),"video")) {
            if (!videoConfigInfo) {
                videoConfigInfo = new VideoConfigInfo ;
                AMediaFormat_getInt32(trackFormat,AMEDIAFORMAT_KEY_WIDTH,&videoConfigInfo->width);
                AMediaFormat_getInt32(trackFormat,AMEDIAFORMAT_KEY_HEIGHT,&videoConfigInfo->height);
                AMediaFormat_getString(trackFormat,AMEDIAFORMAT_KEY_MIME,&videoConfigInfo->videoMime);
                AMediaFormat_getInt32(trackFormat,AMEDIAFORMAT_KEY_FRAME_RATE,&videoConfigInfo->fps);
                AMediaFormat_getInt32(trackFormat,AMEDIAFORMAT_KEY_BIT_RATE,&videoConfigInfo->maxBps);
                videoConfigInfo->toString();
            }
        }
        LOGCATE("打印视频   宽:高   %d:%d",mVideoWidth,mVideoHeight);
        AMediaFormat_delete(trackFormat);
    }
    if (mMediaCodec == nullptr) {
        MsgLoopHelper::sendMsg(Message::obtain(JNI_COMMUNICATE_TYPE_ERROR,0,0,"未能找到编码器"));
        return;
    }
    if (decodeThread == nullptr) decodeThread = new std::thread(&BaseHwDecoder::createDecoderThread, this,mUrl);
    isInit = true;
    LOGCATE("all init over");
}


void BaseHwDecoder::setReadyCall(PrepareCall call){
    this->readyCall = call;
}


void BaseHwDecoder::Destroy() {
    LOGCATE("start to destroy");
    isFinished = true;
    std::unique_lock<std::mutex> pauseLock(mPauseMutex);
    isPaused = false;
    mPauseCondition.notify_all();
    pauseLock.unlock();
    LOGCATE("start to destroy1");
    if (decodeThread) {
        decodeThread->join();
        delete decodeThread;
        decodeThread = nullptr;
    }
    if (mMediaCodec) {
        AMediaCodec_stop(mMediaCodec);
        AMediaCodec_delete(mMediaCodec);
        mMediaCodec = 0;
    }
    if (mMediaExtractor) {
        AMediaExtractor_delete(mMediaExtractor);
        mMediaExtractor = 0;
    }
    if (audioRender){
        audioRender->UnInit();
        delete audioRender;
        audioRender = 0;
    }
    LOGCATE("start to destroy2");
    if (videoRender){
        videoRender->Destroy();
        delete videoRender;
        videoRender = 0;
    }
    if (i420dst) {
        delete [] i420dst;
        i420dst = 0;
    }
    if (videoConfigInfo) {
        delete videoConfigInfo;
        videoConfigInfo = 0;
    }
    if (timeSyncHelper) delete timeSyncHelper;
    isInit = false;
    LOGCATE("has destroy all");
}
void BaseHwDecoder::Start() {
    std::unique_lock<std::mutex> uniqueLock(mPauseMutex);
    isPaused = false;
    if (timeSyncHelper) timeSyncHelper->resetTime();
    mPauseCondition.notify_one();
}
void BaseHwDecoder::Reset() {
    Stop();
    Destroy();
}

void BaseHwDecoder::Replay() {
    std::unique_lock<std::mutex> uniqueLock(mPauseMutex);
    seekProgress = 0;
    mPauseCondition.notify_one();
}

void BaseHwDecoder::Stop() {
    std::unique_lock<std::mutex> uniqueLock(mPauseMutex);
    isPaused = true;
}

void BaseHwDecoder::ManualSeekPosition(int progress) {
    std::unique_lock<std::mutex> uniqueLock(mPauseMutex);
    seekProgress = progress;
}

void BaseHwDecoder::OnSurfaceChanged(int oreration, int windowW, int windowH){
    mWindowWidth = windowW;
    mWindowHeight = windowH;
    this->oreration = oreration;
    OnSizeReady();
}

void BaseHwDecoder::setIfNeedRender(bool needRender){
    this->isNeedRender = needRender;
}

void BaseHwDecoder::setIfNeedPauseWhenFinished(bool needRender){
    this->isNeedPauseWhenFinished = needRender;
}
void BaseHwDecoder::renderAv(int type,AMediaCodecBufferInfo* bufferInfo,uint8_t* data){
//    LOGCATE("开始渲染:%d",type);
    if (type == 1) {
        audioRender->RenderAudioFrame(data,bufferInfo->size);
    } else {
//      mediacodec  输出的nv12 要转换一下
        if (!i420dst) {
            i420dst = new uint8_t [mVideoWidth * mVideoHeight * 3/2];
        }
        memset(i420dst,0x00,mVideoWidth * mVideoHeight * 3/2);
        long long int start = GetSysCurrentTime();
        yuvNv12ToI420(data,i420dst,mVideoWidth,mVideoHeight);
//        LOGCATE("nv12toi420 耗费时间:%lld",(GetSysCurrentTime() - start));
        NativeOpenGLImage openGlImage;
        openGlImage.width = mVideoWidth;
        openGlImage.height = mVideoHeight;
        openGlImage.ppPlane[0] = i420dst;
        openGlImage.ppPlane[1] = i420dst + mVideoWidth * mVideoHeight;
        openGlImage.ppPlane[2] = i420dst + mVideoWidth * mVideoHeight * 5 / 4;
        openGlImage.pLineSize[0]= mVideoWidth;
        openGlImage.pLineSize[1]= mVideoWidth / 2;
        openGlImage.pLineSize[2]= mVideoWidth / 2;
        openGlImage.format = IMAGE_FORMAT_I420;
//        LOGCATE("硬解码渲染的宽 高  %d  :   %d",mVideoWidth,mVideoHeight);
        if (videoRender) videoRender->copyImage(&openGlImage);
    }
}

void BaseHwDecoder::createDecoderThread(const char * url){
    std::unique_lock<std::mutex> surfaceLock(mCreateSurfaceMutex,std::defer_lock);
    surfaceLock.lock();
    if (isNeedRender && appointMediaType == 2 && !videoRender) {
        LOGCATE("videorender has not prepare");
        mSurfaceCondition.wait(surfaceLock);
    }
    surfaceLock.unlock();
    if (readyCall) {
        readyCall(reinterpret_cast<long>(getJniPlayerFromJava()));
    }
    // 发送时长
    while (!isFinished) {
        std::unique_lock<std::mutex> uniqueLock(mPauseMutex,std::defer_lock);
        uniqueLock.lock();
        if (isPaused) {
            mPauseCondition.wait(uniqueLock);
        }
        uniqueLock.unlock();
        std::unique_lock<std::mutex> progressLock(mPauseMutex,std::defer_lock);
        if (progressLock.try_lock()){
            if (seekProgress != -1){
                int64_t seekResult = (int64_t)seekProgress * 1000L * 1000L;
//                LOGCATE("seekto %d   int64:%lld,seekProgress,seekResult);
                media_status_t seekStatus = AMediaExtractor_seekTo(mMediaExtractor,seekResult,
                                                                   AMEDIAEXTRACTOR_SEEK_CLOSEST_SYNC);
                if (seekStatus == AMEDIA_OK) {
                    AMediaCodec_flush(mMediaCodec);
                    timeSyncHelper->resetTime();
                    seekProgress = -1;
                } else {
                    LOGCATE("seek error:%d",seekStatus);
                }

            }
            progressLock.unlock();
        }
        int inIndex = AMediaCodec_dequeueInputBuffer(mMediaCodec,2000);
        if (inIndex >= 0) {
            size_t capacity;
            uint8_t * inputBuffer = AMediaCodec_getInputBuffer(mMediaCodec,inIndex,&capacity);
            size_t inputBufferSize = AMediaExtractor_readSampleData(mMediaExtractor,inputBuffer,capacity);
//            LOGCATE("打印解码完成-----------前的时间戳：%lld",sampleTime);
            if (inputBufferSize > 0) {
                int64_t sampleTime = AMediaExtractor_getSampleTime(mMediaExtractor);
                AMediaCodec_queueInputBuffer(mMediaCodec,inIndex,0,inputBufferSize,sampleTime,0);
//                LOGCATE("打印sampletime：%lld  ",sampleTime);
            } else {
                continue;
            }
//            LOGCATE("读取类型:%d  缓冲大小：%d   capacity:%d   时间戳：%lld",appointMediaType,inputBufferSize,capacity,sampleTime);
            if (!AMediaExtractor_advance(mMediaExtractor)) {
                if (isNeedRender && appointMediaType == 2) {
                    MsgLoopHelper::sendMsg(Message::obtain(JNI_COMMUNICATE_TYPE_COMPLETE,0,0));
                }
                if (onCompleteCall) onCompleteCall->call();
                if (isNeedPauseWhenFinished) {
                    isPaused = true;
                    continue;
                } else {
                    break;
                }
            }
        }
        AMediaCodecBufferInfo codecBufferInfo;
        ssize_t outStatus = AMediaCodec_dequeueOutputBuffer(mMediaCodec, &codecBufferInfo, 0);
        if (outStatus >= 0) {
            std::unique_lock<std::mutex> ptsMutex(audioPtsMutex);
            currentPtsUs = codecBufferInfo.presentationTimeUs;
            ptsMutex.unlock();
            TimeSyncBean timeSyncBean;
            timeSyncBean.syncType = SYNC_TYPE;
            timeSyncBean.currentAudioPtsMs = getCurrentAudioPtsUs() / 1000;
            timeSyncBean.currentVideoPtsMs = codecBufferInfo.presentationTimeUs / 1000;
            if (!timeSyncHelper->hardwareSyncTime(appointMediaType == 1,&timeSyncBean)) {
                LOGCATE("当前慢了 开始跳步:%d",appointMediaType);
                AMediaCodec_flush(mMediaCodec);
                continue;
            }
            uint8_t *outputBuffer = AMediaCodec_getOutputBuffer(mMediaCodec, outStatus, 0);
            // 局部渲染
            if (appointMediaType == 1) {
                int currentSec = codecBufferInfo.presentationTimeUs / (1000 * 1000);
                MsgLoopHelper::sendMsg(
                        Message::obtain(JNI_COMMUNICATE_TYPE_SEEK_PROGRESS_CHANGED, currentSec, 0));
            } else {
//                LOGCATE("打印解码完成后的时间戳：%lld",codecBufferInfo.presentationTimeUs / 1000);
            }
            if (outputDataListener) {
                outputDataListener(appointMediaType,&codecBufferInfo,outputBuffer);
            } else {
                renderAv(appointMediaType,&codecBufferInfo,outputBuffer);
            }
            AMediaCodec_releaseOutputBuffer(mMediaCodec,outStatus, true);
        } else if (outStatus == AMEDIACODEC_INFO_OUTPUT_BUFFERS_CHANGED) {
            LOGCATE("output buffers changed");
        } else if (outStatus == AMEDIACODEC_INFO_OUTPUT_FORMAT_CHANGED) {
            if (appointMediaType == 2) {
                auto format = AMediaCodec_getOutputFormat(mMediaCodec);
                AMediaFormat_getInt32(format,AMEDIAFORMAT_KEY_WIDTH,&mVideoWidth);
                AMediaFormat_getInt32(format,AMEDIAFORMAT_KEY_HEIGHT,&mVideoHeight);
                int colorFormat;
                AMediaFormat_getInt32(format,AMEDIAFORMAT_KEY_COLOR_FORMAT,&colorFormat);
                LOGCATE("打印视频颜色格式:%d  视频宽高:%d - %d",colorFormat,mVideoWidth,mVideoHeight);
                LOGCATE("format changed to: %s", AMediaFormat_toString(format));
                AMediaFormat_getInt32(format,AMEDIAFORMAT_KEY_I_FRAME_INTERVAL,&videoConfigInfo->ifi);
                AMediaFormat_getInt32(format,AMEDIAFORMAT_KEY_COLOR_FORMAT,&videoConfigInfo->colorFormat);
                AMediaFormat_delete(format);
                OnSizeReady();
            }
        } else if (outStatus == AMEDIACODEC_INFO_TRY_AGAIN_LATER) {
//            LOGCATE("no output buffer right now");
        } else {
            LOGCATE("unexpected info code: %zd", outStatus);
        }
//        LOGCATE("loop decode inputIndex:%d    outputIndex:%d",inIndex,outStatus);
    }
    LOGCATE("create thread has over ");
}

BaseHwDecoder::~BaseHwDecoder(){

};

void BaseHwDecoder::OnSizeReady() {
    if (mWindowWidth == 0 || mWindowHeight == 0 || videoRender == nullptr ||
    mVideoWidth == 0 || mVideoHeight == 0) return;
    int m_VideoWidth = mVideoWidth;
    int m_VideoHeight = mVideoHeight;
    int m_RenderWidth,m_RenderHeight;
    setupRenderDimension(oreration,mWindowWidth,mWindowHeight,m_VideoWidth,m_VideoHeight,&m_RenderWidth,&m_RenderHeight);
    auto* render = dynamic_cast<BaseVideoRender *>(videoRender);
    render->OnRenderSizeChanged(mWindowWidth,mWindowHeight,m_RenderWidth,m_RenderHeight);
}

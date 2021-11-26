//
// Created by Admin on 2021/11/22.
//

#include <cstring>
#include "../../../play_header/decoder/hw/BaseHwDecoder.h"
#include <string>
//#include <StringUtils>

BaseHwDecoder::BaseHwDecoder(){
    decodeThread = 0;
    audioRender = 0;
    videoRender = 0;
    isInit = false;
    mMediaCodec = 0;
    mMediaExtractor = 0;
    timeSyncHelper = 0;
}

int64_t BaseHwDecoder::getCurrentAudioPts(){
    return currentPts;
}

void BaseHwDecoder::Init(const char * url) {
    if (isInit) return;
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
        if (startWith(result.c_str(),"video")) {
            AMediaFormat_getInt32(trackFormat,AMEDIAFORMAT_KEY_WIDTH,&mVideoWidth);
            AMediaFormat_getInt32(trackFormat,AMEDIAFORMAT_KEY_HEIGHT,&mVideoHeight);
        }
        // 视音频发送时长
        if (appointMediaType == 1 && startWith(result.c_str(),"audio")) {
            int64_t duration;
            AMediaFormat_getInt64(trackFormat,AMEDIAFORMAT_KEY_DURATION,&duration);
            duration = duration / 1000 / 1000;
            MsgLoopHelper::sendMsg(Message::obtain(JNI_COMMUNICATE_TYPE_DURATION,0,(int)duration));
            LOGCATE("打印视频时长：%d",(int)duration);
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
    if (timeSyncHelper) delete timeSyncHelper;
    isInit = false;
    LOGCATE("has destroy all");
}
void BaseHwDecoder::Start() {
    std::unique_lock<std::mutex> uniqueLock(mPauseMutex);
    isPaused = false;
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
    mPauseCondition.notify_one();
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

void BaseHwDecoder::renderAv(int type,AMediaCodecBufferInfo* bufferInfo,uint8_t* data){
//    LOGCATE("开始渲染:%d",type);
    if (type == 1) {
        audioRender->RenderAudioFrame(data,bufferInfo->size);
    } else {
        NativeOpenGLImage openGlImage;
        openGlImage.width = mVideoWidth;
        openGlImage.height = mVideoHeight;
        openGlImage.ppPlane[0] = data;
        openGlImage.ppPlane[1] = data + mVideoWidth * mVideoHeight;
        openGlImage.ppPlane[2] = data + mVideoWidth * mVideoHeight * 5 / 4;
        openGlImage.pLineSize[0]= mVideoWidth;
        openGlImage.pLineSize[1]= mVideoWidth / 2;
        openGlImage.pLineSize[2]= mVideoWidth / 2;
        openGlImage.format = IMAGE_FORMAT_I420;
        if (videoRender) videoRender->copyImage(&openGlImage);
    }
}

void BaseHwDecoder::createDecoderThread(const char * url){
    std::unique_lock<std::mutex> surfaceLock(mCreateSurfaceMutex,std::defer_lock);
    surfaceLock.lock();
    if (appointMediaType == 2 && !videoRender) {
        LOGCATE("videorender has not prepare");
        mSurfaceCondition.wait(surfaceLock);
    }
    surfaceLock.unlock();
    readyCall(reinterpret_cast<long>(getJniPlayerFromJava()));
    // 发送时长
    while (!isFinished) {
        std::unique_lock<std::mutex> uniqueLock(mPauseMutex,std::defer_lock);
        uniqueLock.lock();
        if (isPaused) {
            mPauseCondition.wait(uniqueLock);
        }
        uniqueLock.unlock();
        int inIndex = AMediaCodec_dequeueInputBuffer(mMediaCodec,2000);
        if (inIndex >= 0) {
            size_t capacity;
            uint8_t * inputBuffer = AMediaCodec_getInputBuffer(mMediaCodec,inIndex,&capacity);
            size_t inputBufferSize = AMediaExtractor_readSampleData(mMediaExtractor,inputBuffer,capacity);
            int64_t sampleTime = AMediaExtractor_getSampleTime(mMediaExtractor);
            if (inputBufferSize > 0 && sampleTime > -1) {
                std::unique_lock<std::mutex> ptsMutex(audioPtsMutex);
                currentPts = sampleTime;
                ptsMutex.unlock();
                TimeSyncBean timeSyncBean;
                timeSyncBean.syncType = 1;
                timeSyncBean.currentAudioPts = getCurrentAudioPts() / 1000;
                timeSyncBean.currentVideoPts = sampleTime / 1000;
                if (!timeSyncHelper->hardwareSyncTime(appointMediaType == 1,&timeSyncBean)) {
                    LOGCATE("当前慢了 开始跳步:%d",appointMediaType);
                    AMediaCodec_flush(mMediaCodec);
                    AMediaExtractor_advance(mMediaExtractor);
                    continue;
                }
                AMediaCodec_queueInputBuffer(mMediaCodec,inIndex,0,inputBufferSize,sampleTime,0);
            } else {
                continue;
            }
//            LOGCATE("读取类型:%d  缓冲大小：%d   capacity:%d   时间戳：%lld",appointMediaType,inputBufferSize,capacity,sampleTime);
            if (!AMediaExtractor_advance(mMediaExtractor)) {
                isPaused = true;
                if (appointMediaType == 2) {
                    MsgLoopHelper::sendMsg(Message::obtain(JNI_COMMUNICATE_TYPE_COMPLETE,0,0));
                }
                continue;
            }
        }
        AMediaCodecBufferInfo codecBufferInfo;
        ssize_t outStatus = AMediaCodec_dequeueOutputBuffer(mMediaCodec, &codecBufferInfo, 0);
        if (outStatus >= 0) {
            uint8_t *outputBuffer = AMediaCodec_getOutputBuffer(mMediaCodec, outStatus, 0);
            // 局部渲染
            if (appointMediaType == 1) {
                int currentSec = codecBufferInfo.presentationTimeUs / (1000 * 1000);
                MsgLoopHelper::sendMsg(Message::obtain(JNI_COMMUNICATE_TYPE_SEEK_PROGRESS_CHANGED,currentSec,0));
            }
            renderAv(appointMediaType,&codecBufferInfo,outputBuffer);
            AMediaCodec_releaseOutputBuffer(mMediaCodec,outStatus, false);
        } else if (outStatus == AMEDIACODEC_INFO_OUTPUT_BUFFERS_CHANGED) {
            LOGCATE("output buffers changed");
        } else if (outStatus == AMEDIACODEC_INFO_OUTPUT_FORMAT_CHANGED) {
            auto format = AMediaCodec_getOutputFormat(mMediaCodec);
            LOGCATE("format changed to: %s", AMediaFormat_toString(format));
            AMediaFormat_delete(format);
        } else if (outStatus == AMEDIACODEC_INFO_TRY_AGAIN_LATER) {
//            LOGCATE("no output buffer right now");
        } else {
            LOGCATE("unexpected info code: %zd", outStatus);
        }
//        LOGCATE("loop decode inputIndex:%d    outputIndex:%d",inIndex,outStatus);
        std::unique_lock<std::mutex> progressLock(mPauseMutex,std::defer_lock);
        if (progressLock.try_lock()){
            if (seekProgress != -1){
                LOGCATE("seekto ");
                AMediaExtractor_seekTo(mMediaExtractor,seekProgress * 1000L * 1000L,AMEDIAEXTRACTOR_SEEK_PREVIOUS_SYNC );
                AMediaCodec_flush(mMediaCodec);
                timeSyncHelper->resetTime();
                seekProgress = -1;
            }
            progressLock.unlock();
        }
    }
    LOGCATE("create thread has over ");
}

void BaseHwDecoder::OnSizeReady() {
    if (mWindowWidth == 0 || mWindowHeight == 0 || videoRender == nullptr) return;
    int m_VideoWidth = mVideoWidth;
    int m_VideoHeight = mVideoHeight;
    int m_RenderWidth,m_RenderHeight;
    setupRenderDimension(oreration,mWindowWidth,mWindowHeight,m_VideoWidth,m_VideoHeight,&m_RenderWidth,&m_RenderHeight);
    videoRender->OnRenderSizeChanged(mWindowWidth,mWindowHeight,m_RenderWidth,m_RenderHeight);
}

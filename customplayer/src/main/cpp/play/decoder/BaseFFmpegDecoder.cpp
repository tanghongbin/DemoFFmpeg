#include <decoder/BaseFFmpegDecoder.h>

#include <thread>
#include <utils/MsgLoopHelper.h>
#include <render/FFmpegAudioConverter.h>
#include <decoder/ImlFFmpegDecoder.h>
#include <render/FFmpegVideoToRgbConverter.h>
#include <utils/TimeSyncHelper.h>

//
// Created by Admin on 2021/5/24.
//

#define SYNC_TYPE_FFMPEG_TIME 1 // 时间戳同步

void BaseFFmpegDecoder::Init(const char * url){
    LOGCATE("base prepare decode");
    if (appointMediaType == 0){
        LOGCATE("you must setup appoing type");
        exit(0);
    }
    isRunning = true;
    if (appointMediaType == 1 && !audioRender){
        audioRender = new OpenSLESRender;
        audioRender->Init();
    }
    mUrl = url;
    readThread = new std::thread(BaseFFmpegDecoder::createReadThread, this);
}

void BaseFFmpegDecoder::resolveConvertResult(void * decoder, void * data, int size){
//    LOGCATE("resolveConvertResult receive data");
    auto* baseDecoder = static_cast<BaseFFmpegDecoder *>(decoder);
    if (baseDecoder->appointMediaType == 2 && baseDecoder->videoRender){
        auto *resultData = static_cast<NativeOpenGLImage *>(data);
        LOGCATE("打印ffmpeg 转换的宽高：%d : %d",resultData->width,resultData->height);
        baseDecoder->videoRender->copyImage(resultData);
    } else if (baseDecoder->appointMediaType == 1 && baseDecoder->audioRender) {
        baseDecoder->audioRender->RenderAudioFrame(static_cast<uint8_t *>(data), size);
    }
}

void BaseFFmpegDecoder::createReadThread(BaseFFmpegDecoder *baseDecoder) {
    AVFormatContext *formatCtx = avformat_alloc_context();
    int ret;
    std::string result = "";
    AVMediaType mediaType;
    int currentStreamIndex;
    AVCodecContext * codeCtx = nullptr;
    AVCodec *codeC;
    if (!formatCtx){
        ret = -1;
        result = "avformat_alloc_context falied";
        goto EndRecycle;
    }
    ret = avformat_open_input(&formatCtx,baseDecoder->mUrl,NULL,NULL);
    if (ret < 0){
        result = av_err2str(ret);
        goto EndRecycle;
    }
    ret = avformat_find_stream_info(formatCtx,NULL);
    if (ret < 0){
        result = av_err2str(ret);
        goto EndRecycle;
    }
    mediaType = baseDecoder->appointMediaType == 1 ? AVMEDIA_TYPE_AUDIO : AVMEDIA_TYPE_VIDEO;
    for (int i = 0; i < formatCtx->nb_streams; ++i) {
        if (mediaType == formatCtx->streams[i]->codecpar->codec_type){
            currentStreamIndex = i;
        }
    }
    codeC = avcodec_find_decoder(formatCtx->streams[currentStreamIndex]->codecpar->codec_id);
    if (!codeC){
        LOGCATE("this type:%d  codeId:%d h264:%d",mediaType
                ,formatCtx->streams[currentStreamIndex]->codecpar->codec_id,AV_CODEC_ID_H264);
        result = "can't find codec";
        ret = -1;
        goto EndRecycle;
    }
    codeCtx = baseDecoder -> codeCtx = avcodec_alloc_context3(codeC);
    if (!codeCtx){
        result = "can't avcodec_alloc_context3";
        ret = -1;
        goto EndRecycle;
    }
    ret = avcodec_parameters_to_context(codeCtx,
                                        formatCtx->streams[currentStreamIndex]->codecpar);
    if (checkNegativeReturn(ret, "can't attach params to decodeCtx")) return;

    ret = avcodec_open2(codeCtx,codeC,NULL);
    if (ret < 0){
        result = av_err2str(ret);
        goto EndRecycle;
    }
    baseDecoder->OnDecodeReady(formatCtx, currentStreamIndex);
    // start loop
    baseDecoder->decodeLoop(formatCtx, codeCtx, currentStreamIndex, baseDecoder);
    
    EndRecycle:
    if (ret < 0) {
        MsgLoopHelper::sendMsg(Message::obtain(JNI_COMMUNICATE_TYPE_ERROR,ret,0,result));
    }
    if (formatCtx) {
        avformat_close_input(&formatCtx);
        avformat_free_context(formatCtx);
    }
    if (codeCtx){
        avcodec_free_context(&codeCtx);
    }
    LOGCATE("all ctx are freed,the thread has end");
}

void BaseFFmpegDecoder::decodeLoop(AVFormatContext *pContext, AVCodecContext *pCodecContext, int stream_index,
                                   BaseFFmpegDecoder *baseDecoder) {
    // alloc packet,frame
    AVPacket *packet = av_packet_alloc();
    AVFrame *frame = av_frame_alloc();
    timeSyncHelper = new TimeSyncHelper;
    baseDecoder -> mDataConverter = createConverter();
    if (pContext->streams[stream_index]->codecpar->codec_type == AVMEDIA_TYPE_VIDEO){
        std::unique_lock<std::mutex> uniqueLock(createSurfaceMutex);
       if (!baseDecoder->videoRender){
           LOGCATE("before wait decodeLoop videoRender is null:%p",baseDecoder->videoRender);
           createSurfaceCondition.wait(uniqueLock);
           LOGCATE("after wait decodeLoop videoRender is null:%p",baseDecoder->videoRender);
       }
        uniqueLock.unlock();
        auto* videoDecoder = dynamic_cast<VideoFFmpegDecoder *>(baseDecoder);
        videoDecoder -> OnSizeReady();
       LOGCATE("decodeLoop set render success:%p",baseDecoder->videoRender);
    }
    baseDecoder->mDataConverter->convertResult = BaseFFmpegDecoder::resolveConvertResult;
    baseDecoder->mDataConverter->baseDecoder = this;
    baseDecoder -> mDataConverter ->Init(pCodecContext);
    int ret;
    call(reinterpret_cast<long>(getJniPlayerFromJava()));
    int defaultSyncTimeType = SYNC_TYPE_FFMPEG_TIME; // 视频向音频同步
    while (isRunning){
        // pause
        std::unique_lock<std::mutex> uniqueLock(customMutex);
        if (!isStarted){
            LOGCATE("start to wait");
            condition.wait(uniqueLock);
            LOGCATE("start to wait is over");
        }
        uniqueLock.unlock();

        // seek to target
        if (mManualSeekPosition != -1){
            int64_t targetPosition = mManualSeekPosition * AV_TIME_BASE;
            ret = avformat_seek_file(pContext, -1, INT64_MIN, targetPosition, INT64_MAX, 0);
            if (ret == 0){
                avcodec_flush_buffers(pCodecContext);
                timeSyncHelper->resetTime();
            } else {
                LOGCATE("error seek file:%s",av_err2str(ret));
            }
            mManualSeekPosition = -1;
        }

        ret = av_read_frame(pContext, packet);
        if (ret == AVERROR_EOF){
            LOGCATE("start to read AVERROR_EOF %d",ret);
            if (pContext->streams[stream_index]->codecpar->codec_type == AVMEDIA_TYPE_VIDEO) {
                MsgLoopHelper::sendMsg(Message::obtain(JNI_COMMUNICATE_TYPE_COMPLETE,0,0));
            }
            Stop();
            continue;
        }else if(ret < 0){
            LOGCATE("start to read error  %d",ret);
            MsgLoopHelper::sendMsg(Message::obtain(JNI_COMMUNICATE_TYPE_ERROR,ret,0,av_err2str(ret)));
            goto DecodeLoopEnd;
        }

        if (packet->stream_index == stream_index){
            TimeSyncBean dtsBean;
            dtsBean.syncType = defaultSyncTimeType;
            dtsBean.currentAudioPtsMs = getCurrentAudioPtsUs();
            if (!timeSyncHelper->syncTime(true,packet,frame,pContext,stream_index,&dtsBean)){
                continue;
            }
            ret = avcodec_send_packet(pCodecContext, packet);
            if (ret < 0){
                LOGCATE("avcodec_send_packet error:%s",av_err2str(ret));
                goto innerEnd;
            }
            // caculate timestamp only audio
            if (pContext->streams[stream_index]->codecpar->codec_type == AVMEDIA_TYPE_AUDIO){
                double currentDtsDuration =
                        packet->dts * av_q2d(pContext->streams[stream_index]->time_base);
                int currentProgress = (int)currentDtsDuration;
                currentAudioPts = packet->dts * av_q2d(pContext -> streams[stream_index]->time_base) * 1000;
//                LOGCATE("current time:%d",currentProgress);
                MsgLoopHelper::sendMsg(Message::obtain(JNI_COMMUNICATE_TYPE_SEEK_PROGRESS_CHANGED,currentProgress,0));
            }
            while (avcodec_receive_frame(pCodecContext, frame) == 0){
//                LOGCATE("avcodec_receive_frame receive success");
                TimeSyncBean ptsBean;
                ptsBean.syncType = defaultSyncTimeType;
                ptsBean.currentAudioPtsMs = getCurrentAudioPtsUs();
                if (!timeSyncHelper->syncTime(false,packet,frame,pContext,stream_index,&ptsBean)){
                    goto innerEnd;
                }
                DataConvertInfo dataConvertInfo;
                dataConvertInfo.frameData = frame;
                baseDecoder -> mDataConverter ->covertData(&dataConvertInfo);
            }
        }
        innerEnd:
        av_packet_unref(packet);
    }

    DecodeLoopEnd:
    LOGCATE("loop has end ret:%d errorStr:%s",ret,av_err2str(ret));
    baseDecoder -> mDataConverter ->Destroy();
    delete baseDecoder -> mDataConverter;
    baseDecoder -> mDataConverter = nullptr;
    av_packet_unref(packet);
    av_packet_free(&packet);
    av_frame_free(&frame);
}

void BaseFFmpegDecoder::Destroy(){
    if (videoRender){
        videoRender->Destroy();
        delete videoRender;
        videoRender = 0;
    }
    isRunning = false;
    condition.notify_one();
    if (readThread) {
        readThread->join();
        delete readThread;
        readThread = nullptr;
    }
    if (audioRender) {
        audioRender->UnInit();
        delete audioRender;
        audioRender = 0;
    }
    if (timeSyncHelper) {
        delete timeSyncHelper;
        timeSyncHelper = 0;
    }
}

void BaseFFmpegDecoder::Reset(){
    Stop();
    isRunning = false;
    condition.notify_one();
    if (readThread) {
        readThread->join();
        delete readThread;
        readThread = nullptr;
    }
}

void BaseFFmpegDecoder::Start(){
    std::lock_guard<std::mutex> lockGuard(customMutex);
    isStarted = true;
    timeSyncHelper->resetTime();
    condition.notify_one();
}

void BaseFFmpegDecoder::Stop(){
    std::lock_guard<std::mutex> lockGuard(customMutex);
    isStarted = false;
}

void BaseFFmpegDecoder::ManualSeekPosition(int position){
    std::lock_guard<std::mutex> lockGuard(customMutex);
    mManualSeekPosition = position;
}

void BaseFFmpegDecoder::Replay(){
    ManualSeekPosition(0);
    Start();
}

BaseFFmpegDecoder::BaseFFmpegDecoder(){
    readThread = 0;
    isRunning = true;
    isStarted = false;
    mDataConverter = 0;
    videoRender = 0;
    audioRender = 0;
    mDataConverter = 0;
    mManualSeekPosition = -1;
    currentAudioPts = 0LL;
    timeSyncHelper = 0;
}

void BaseFFmpegDecoder::OnDecodeReady(AVFormatContext *pContext, int streamIndex) {
    int64_t result = pContext->duration / (1000 * 1000);
    LOGCATE("audio duration is :%lld",result);
    if (pContext->streams[streamIndex]->codecpar->codec_type == AVMEDIA_TYPE_AUDIO){
        MsgLoopHelper::sendMsg(Message::obtain(JNI_COMMUNICATE_TYPE_DURATION,0,(int)result));
    }
}

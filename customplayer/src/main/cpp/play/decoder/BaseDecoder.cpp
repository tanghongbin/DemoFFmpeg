#include <decoder/BaseDecoder.h>

#include <thread>
#include <utils/MsgLoopHelper.h>
#include <render/AudioDataConverter.h>
#include <decoder/ImlDecoder.h>
#include <render/VideoDataConverter.h>

//
// Created by Admin on 2021/5/24.
//



void BaseDecoder::Init(const char * url){
    LOGCATE("base prepare decode");
    if (appointMediaType == 0){
        LOGCATE("you must setup appoing type");
        exit(0);
    }
    mUrl = url;
    readThread = new std::thread(BaseDecoder::createReadThread, this);
}

void BaseDecoder::createReadThread(BaseDecoder *baseDecoder) {
    AVFormatContext *formatCtx = avformat_alloc_context();
    int ret;
    std::string result = "";
    AVMediaType mediaType;
    int currentStreamIndex;
    AVCodec *codeC;
    AVCodecContext *codeCtx = nullptr;
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
        result = "can't find codec";
        ret = -1;
        goto EndRecycle;
    }
    codeCtx = avcodec_alloc_context3(codeC);
    if (!codeCtx){
        result = "can't find codec";
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
    LOGCATE("all ctx are freed");
}

void BaseDecoder::decodeLoop(AVFormatContext *pContext, AVCodecContext *pCodecContext, int stream_index,
                             BaseDecoder *baseDecoder) {
    // alloc packet,frame
    AVPacket *packet = av_packet_alloc();
    AVFrame *frame = av_frame_alloc();
    baseDecoder -> mDataConverter = createConverter();
    if (pContext->streams[stream_index]->codecpar->codec_type == AVMEDIA_TYPE_VIDEO){
       VideoDecoder* videoResult = dynamic_cast<VideoDecoder *>(baseDecoder);
       VideoDataConverter * videoConverter = dynamic_cast<VideoDataConverter *>(baseDecoder->mDataConverter);
        std::unique_lock<std::mutex> uniqueLock(createSurfaceMutex);
       if (!videoResult->videoRender){
           LOGCATE("before wait decodeLoop videoRender is null:%p",videoResult->videoRender);
           createSurfaceCondition.wait(uniqueLock);
           LOGCATE("after wait decodeLoop videoRender is null:%p",videoResult->videoRender);
       }
       uniqueLock.unlock();
       videoConverter->videoRender = videoResult->videoRender;
       LOGCATE("decodeLoop set render success:%p",videoResult->videoRender);
    }
    baseDecoder -> mDataConverter ->Init(pCodecContext);
    int ret;
    call(reinterpret_cast<long>(getJniPlayerFromJava()));
    while (isRunning){
        // pause
        std::unique_lock<std::mutex> uniqueLock(customMutex);
        if (!isStarted){
            LOGCATE("start to wait");
            condition.wait(uniqueLock);
        }
        uniqueLock.unlock();

        // seek to target
        if (mManualSeekPosition != -1){
            int64_t targetPosition = mManualSeekPosition * AV_TIME_BASE;
            ret = avformat_seek_file(pContext, -1, INT64_MIN, targetPosition, INT64_MAX, 0);
            if (ret == 0){
                avcodec_flush_buffers(pCodecContext);
            } else {
                LOGCATE("error seek file:%s",av_err2str(ret));
            }
            mManualSeekPosition = -1;
        }

        ret = av_read_frame(pContext, packet);
        if (ret == AVERROR_EOF){
            LOGCATE("start to read AVERROR_EOF %d",ret);
            MsgLoopHelper::sendMsg(Message::obtain(JNI_COMMUNICATE_TYPE_COMPLETE,0,0));
            goto DecodeLoopEnd;
        }else if(ret < 0){
            LOGCATE("start to read error  %d",ret);
            MsgLoopHelper::sendMsg(Message::obtain(JNI_COMMUNICATE_TYPE_ERROR,ret,0,av_err2str(ret)));
            goto DecodeLoopEnd;
        }

        if (packet->stream_index == stream_index){
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
//                LOGCATE("current time:%d",currentProgress);
                MsgLoopHelper::sendMsg(Message::obtain(JNI_COMMUNICATE_TYPE_SEEK_PROGRESS_CHANGED,currentProgress,0));
            }
            while (avcodec_receive_frame(pCodecContext, frame) == 0){
//                LOGCATE("avcodec_receive_frame receive success");
                baseDecoder -> mDataConverter ->covertData(frame);
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

void BaseDecoder::Destroy(){
    isRunning = false;
    condition.notify_one();
    readThread->join();
    delete readThread;
    readThread = nullptr;
}

void BaseDecoder::Start(){
    std::lock_guard<std::mutex> lockGuard(customMutex);
    isStarted = true;
    condition.notify_one();
}

void BaseDecoder::Stop(){
    std::lock_guard<std::mutex> lockGuard(customMutex);
    isStarted = false;
}

void BaseDecoder::ManualSeekPosition(int position){
    std::lock_guard<std::mutex> lockGuard(customMutex);
    mManualSeekPosition = position;
}


void BaseDecoder::OnDecodeReady(AVFormatContext *pContext, int streamIndex) {
    int64_t result = pContext->duration / (1000 * 1000);
    LOGCATE("audio duration is :%lld",result);
    if (pContext->streams[streamIndex]->codecpar->codec_type == AVMEDIA_TYPE_AUDIO){
        MsgLoopHelper::sendMsg(Message::obtain(JNI_COMMUNICATE_TYPE_DURATION,0,(int)result));
    }
}

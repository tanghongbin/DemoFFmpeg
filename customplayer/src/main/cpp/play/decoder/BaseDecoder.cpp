#include <decoder/BaseDecoder.h>

#include <thread>
#include <utils/MsgLoopHelper.h>
#include <render/AudioDataConverter.h>

//
// Created by Admin on 2021/5/24.
//



void BaseDecoder::Init(const char * url){
    LOGCATE("base prepare decode");
    if (appointMediaType == 0){
        LOGCATE("you must setup appoing type");
        exit(0);
        return;
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

    // start loop
    baseDecoder->decodeLoop(formatCtx, codeCtx, currentStreamIndex);
    
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

void BaseDecoder::decodeLoop(AVFormatContext *formatCtx, AVCodecContext *codeCtx, int streamIndex) {
    // alloc packet,frame
    AVPacket *packet = av_packet_alloc();
    AVFrame *frame = av_frame_alloc();
    BaseDataCoverter* dataConverter = new AudioDataConverter;
    dataConverter->Init(codeCtx);
    int ret;
    call(reinterpret_cast<long>(getJniPlayerFromJava()));
    while (isRunning){
        // pause
//        std::unique_lock<std::mutex> lockGuard(customMutex);
//        if (!isStarted){
//            LOGCATE("start to wait");
//            condition.wait(lockGuard);
//        }
//        lockGuard.unlock();

        ret = av_read_frame(formatCtx,packet);
        if (ret == AVERROR_EOF){
            LOGCATE("start to read AVERROR_EOF %d",ret);
            MsgLoopHelper::sendMsg(Message::obtain(JNI_COMMUNICATE_TYPE_COMPLETE,0,0));
            goto EndRecycle;
        }else if(ret < 0){
            LOGCATE("start to read error  %d",ret);
            MsgLoopHelper::sendMsg(Message::obtain(JNI_COMMUNICATE_TYPE_ERROR,ret,0,av_err2str(ret)));
            goto EndRecycle;
        }

        if (packet->stream_index == streamIndex){
            ret = avcodec_send_packet(codeCtx,packet);
            if (ret < 0){
                LOGCATE("avcodec_send_packet error:%s",av_err2str(ret));
                goto innerEnd;
            }
            while (avcodec_receive_frame(codeCtx,frame) == 0){
//                LOGCATE("avcodec_receive_frame receive success");
                dataConverter->covertData(frame);
            }
        }
        innerEnd:
        av_packet_unref(packet);
    }

    EndRecycle:
    LOGCATE("loop has end ret:%d errorStr:%s",ret,av_err2str(ret));
    dataConverter->Destroy();
    delete dataConverter;
    av_packet_unref(packet);
    av_packet_free(&packet);
    av_frame_free(&frame);
}

void BaseDecoder::destroyThread(){
    readThread->join();
    delete readThread;
    readThread = nullptr;
}

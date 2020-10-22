//
// Created by Admin on 2020/10/19.
//

#include <utils.h>
#include "PlayMp4Practice.h"
#include "BaseRender.h"
#include "AudioRender.h"
#include "VideoRender.h"
#include "CustomGLUtils.h"

extern "C" {
#include "../include/libavcodec/avcodec.h"
#include "../include/libavutil/frame.h"
#include "../include/libavformat/avformat.h"
};


void PlayMp4Practice::init(const char *url, JNIEnv *jniEnv, jobject renderInstance,
                           jobject surface, int type) {
    strcpy(mUrl, url);
    mRenderInstance = jniEnv->NewGlobalRef(renderInstance);
    mSurface = jniEnv->NewGlobalRef(surface);
    LOGCATE("play address is:%s", mUrl);
    mThread = new thread(createPlayProcess, this,mRenderInstance, mSurface, type);
    LOGCATE("print thread name:%s", mThread->get_id());
}


void
PlayMp4Practice::createPlayProcess(PlayMp4Practice *pPractice, jobject renderInstance, jobject surface,
                                   int type) {
    // 1-音频，2-视频
    AVFormatContext *avFormatContext;
    AVCodec *codec;
    int ret = -1;
    int stream_index;
    AVCodecContext *codecContext;

    LOGCATE("native print play url:%s",pPractice->mUrl);
    avFormatContext = avformat_alloc_context();
    if (!avFormatContext) {
        LOGCATE("avformat alloc failed");
        return;
    }
    ret = avformat_open_input(&avFormatContext, pPractice->mUrl, NULL, NULL);
    if (checkNegativeReturn(ret, "open input failed")) return;

    ret = avformat_find_stream_info(avFormatContext, NULL);
    if (checkNegativeReturn(ret, "find stream info failed")) return;
    AVMediaType mediaType = type == 1 ? AVMEDIA_TYPE_AUDIO : AVMEDIA_TYPE_VIDEO;

    for (int i = 0; i < avFormatContext->nb_streams; ++i) {
        if (mediaType == avFormatContext->streams[i]->codecpar->codec_type) {
            stream_index = i;
        }
    }
    // 找到解码器
    codec = avcodec_find_decoder(avFormatContext->streams[stream_index]->codecpar->codec_id);
    if (!codec) {
        LOGCATE("can't find decoder");
        return;
    }
    // 创建解码环境
    codecContext = avcodec_alloc_context3(codec);
    if (!codecContext) {
        LOGCATE("can't alloc code context");
        return;
    }
    ret = avcodec_parameters_to_context(codecContext,avFormatContext->streams[stream_index]->codecpar);
    if (checkNegativeReturn(ret, "can't attach params to decodeCtx")) return;

    ret = avcodec_open2(codecContext,codec,NULL);
    if (checkNegativeReturn(ret, "open decoder failed")) return;

    BaseRender *baseRender = pPractice->createRender(type);
    baseRender->init(codecContext,renderInstance,surface);
    AVPacket *packet = av_packet_alloc();
    AVFrame *frame = av_frame_alloc();
    pPractice->decodeLoop(packet, frame, baseRender, renderInstance, surface, codecContext,
                          avFormatContext, stream_index);

    baseRender->unInit();
    delete baseRender;
    av_packet_free(&packet);
    av_frame_free(&frame);

    avformat_close_input(&avFormatContext);
    avcodec_free_context(&codecContext);
    avformat_free_context(avFormatContext);
}

BaseRender* PlayMp4Practice::createRender(int type) {
    if (type == 1) {
        return new AudioRender;
    } else if (type == 2) {
        return new VideoRender;
    } else {
        return new VideoRender;
    }

}



void PlayMp4Practice::decodeLoop(AVPacket *pPacket, AVFrame *pFrame, BaseRender *pRender,
                                 jobject pJobject,
                                 jobject pJobject1, AVCodecContext *codecContext,
                                 AVFormatContext *pContext, int stream_index) {

    // 循环解析
    LOGCATE("prepare play");
    int ret;
    for (;;){
        ret = av_read_frame(pContext,pPacket);
        if (ret < 0) {
            LOGCATE("av_read_frame finished or some other error");
            break;
        }
        LOGCATE("log streamIndex:%d",pPacket->stream_index);
        if (pPacket->stream_index == stream_index){
            ret = avcodec_send_packet(codecContext,pPacket);
            if (ret < 0) {
                LOGCATE("avcodec_send_packet finished or some other error");
                break;
            }
            pRender->eachPacket(pPacket,codecContext);
            while (avcodec_receive_frame(codecContext,pFrame) == 0){
                pRender->draw_frame(codecContext, pFrame, pJobject1);
            }
        }
        av_packet_unref(pPacket);
    }
    LOGCATE("loop end");
}

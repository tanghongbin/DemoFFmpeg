//
// Created by Admin on 2020/10/19.
//

#include <utils.h>
#include <helpers/TimeAsyncHelper.h>
#include <filters/WaterFilterHelper.h>
#include <encode/EncodeYuvToJpg.h>
#include "PlayMp4Practice.h"
#include "BaseRender.h"
#include "AudioRender.h"
#include "VideoRender.h"
#include "CustomGLUtils.h"
#include "OpenGLFFmpegDecoder.h"

extern "C" {
#include <libavfilter/buffersrc.h>
#include <libavfilter/buffersink.h>
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
    mThread = new thread(createPlayProcess, this, mRenderInstance, mSurface, type);
    LOGCATE("print thread name:%s", mThread->get_id());
}


void
PlayMp4Practice::createPlayProcess(PlayMp4Practice *pPractice, jobject renderInstance,
                                   jobject surface,
                                   int type) {
    // 1-音频，2-视频
    AVFormatContext *avFormatContext;
    AVCodec *codec;
    int ret = -1;
    int stream_index = -1;
    AVCodecContext *codecContext;

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
            LOGCATE("当前type:%d  stream_index:%d", type, stream_index);
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
    ret = avcodec_parameters_to_context(codecContext,
                                        avFormatContext->streams[stream_index]->codecpar);
    if (checkNegativeReturn(ret, "can't attach params to decodeCtx")) return;

    ret = avcodec_open2(codecContext, codec, NULL);
    if (checkNegativeReturn(ret, "open decoder failed")) return;

    BaseRender *baseRender = pPractice->createRender(type);
    baseRender->init(codecContext, renderInstance, surface);
    AVPacket *packet = av_packet_alloc();
    AVFrame *frame = av_frame_alloc();
    if (ENABLE_AV_FILTER) {
        bool isVideo =
                avFormatContext->streams[stream_index]->codecpar->codec_type == AVMEDIA_TYPE_VIDEO;
        // 只有视频使用滤镜，音频不用
        if (isVideo) {
            LOGCATE("isvideo:true index:%d", stream_index);
            pPractice->decodeLoopWithFilter(packet, frame, baseRender, renderInstance, surface,
                                            codecContext,
                                            avFormatContext, stream_index);
        } else {
            pPractice->decodeLoop(packet, frame, baseRender, renderInstance, surface, codecContext,
                                  avFormatContext, stream_index);
        }
    } else {
        pPractice->decodeLoop(packet, frame, baseRender, renderInstance, surface, codecContext,
                              avFormatContext, stream_index);
    }

    baseRender->unInit();
    delete baseRender;
    av_packet_free(&packet);
    av_frame_free(&frame);

    avformat_close_input(&avFormatContext);
    avcodec_free_context(&codecContext);
    avformat_free_context(avFormatContext);
    LOGCATE("free all context over");
}

BaseRender *PlayMp4Practice::createRender(int type) {
    if (type == 1) {
        return new AudioRender;
    } else if (type == 2) {
        return new VideoRender;
    } else if (type == 4) {
        return new OpenGLFFmpegDecoder;
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
    TimeAsyncHelper *asyncHelper = new TimeAsyncHelper;
    int ret;
    while (isLoop) {
        ret = av_read_frame(pContext, pPacket);
        if (ret < 0) {
            LOGCATE("av_read_frame error:%s", av_err2str(ret));
            break;
        }
//        LOGCATE("log streamIndex:%d", pPacket->stream_index);
        if (pPacket->stream_index == stream_index) {
            asyncHelper->updateTimeStamp(true, pPacket, NULL, pContext, stream_index);
            if (!asyncHelper->assertDtsIsValid()) {
                goto LoopEnd;
            }
            ret = avcodec_send_packet(codecContext, pPacket);
            if (ret < 0) {
                LOGCATE("avcodec_send_packet error:%s", av_err2str(ret));
                break;
            }
            pRender->eachPacket(pPacket, codecContext);
            while (avcodec_receive_frame(codecContext, pFrame) == 0) {
                asyncHelper->updateTimeStamp(false, pPacket, pFrame, pContext, stream_index);
                asyncHelper->async();
                pRender->draw_frame(codecContext, pFrame, pJobject1);
            }
        }
        LoopEnd:
        av_packet_unref(pPacket);
    }
    delete asyncHelper;
    asyncHelper = nullptr;
    LOGCATE("loop end");
}

void PlayMp4Practice::decodeLoopWithFilter(AVPacket *pPacket, AVFrame *pFrame, BaseRender *pRender,
                                           jobject pJobject,
                                           jobject pJobject1, AVCodecContext *codecContext,
                                           AVFormatContext *pContext, int stream_index) {

    // 循环解析
    LOGCATE("prepare play");
    const char *outName = getRandomStr("frameyuvout-", ".yuv");
    LOGCATE("log yuv random address :%s",outName);
    FILE *fp_yuv = fopen(outName, "wb+");
    if (!fp_yuv) {
        LOGCATE("open outfile failed");
        return;
    }

    TimeAsyncHelper *asyncHelper = new TimeAsyncHelper;
    WaterFilterHelper *waterFilterHelper = new WaterFilterHelper;
    int ret;
//    ret = waterFilterHelper->initWaterFilter(pContext, codecContext, stream_index);
    if (ret < 0) {
        LOGCATE("waterfilterHelper init failed:%s",av_err2str(ret));
        goto WholeLoopEnd;
    }
    while (isLoop) {
        ret = av_read_frame(pContext, pPacket);
        if (ret < 0) {
            LOGCATE("av_read_frame error:%s", av_err2str(ret));
            break;
        }
//        LOGCATE("log streamIndex:%d", pPacket->stream_index);
        if (pPacket->stream_index == stream_index) {
            asyncHelper->updateTimeStamp(true, pPacket, NULL, pContext, stream_index);
            if (!asyncHelper->assertDtsIsValid()) {
                goto StartNextLoop;
            }
            ret = avcodec_send_packet(codecContext, pPacket);
            if (ret < 0) {
                LOGCATE("avcodec_send_packet error:%s", av_err2str(ret));
                break;
            }
            pRender->eachPacket(pPacket, codecContext);
            while (avcodec_receive_frame(codecContext, pFrame) == 0) {
                //------------------ 添加滤镜
                if (av_buffersrc_add_frame_flags(waterFilterHelper->buffersrc_ctx, pFrame,
                                                 AV_BUFFERSRC_FLAG_KEEP_REF) < 0) {
                    av_log(NULL, AV_LOG_ERROR, "Error while feeding the filtergraph\n");
                    break;
                }
                while (true) {
//                    ret = av_buffersink_get_frame(waterFilterHelper->buffersink_ctx,
//                                                  waterFilterHelper->filterFrame);
                    if (ret == AVERROR(EAGAIN) || ret == AVERROR_EOF) {
                        break;
                    }
                    if (ret < 0) {
                        goto WholeLoopEnd;
                    }
//                    AVFrame *pFrame_out = waterFilterHelper->filterFrame;
//                    if (pFrame_out->format == AV_PIX_FMT_YUV420P) {
//                        //Y, U, V
//                        for (int i = 0; i < pFrame_out->height; i++) {
//                            fwrite(pFrame_out->data[0] + pFrame_out->linesize[0] * i, 1,
//                                   pFrame_out->width, fp_yuv);
//                        }
//                        for (int i = 0; i < pFrame_out->height / 2; i++) {
//                            fwrite(pFrame_out->data[1] + pFrame_out->linesize[1] * i, 1,
//                                   pFrame_out->width / 2, fp_yuv);
//                        }
//                        for (int i = 0; i < pFrame_out->height / 2; i++) {
//                            fwrite(pFrame_out->data[2] + pFrame_out->linesize[2] * i, 1,
//                                   pFrame_out->width / 2, fp_yuv);
//
//                        }
//                        LOGCATE("write yuv data over");
//                    }

//                    asyncHelper->updateTimeStamp(false, pPacket, waterFilterHelper->filterFrame,
//                                                 pContext, stream_index);
//                    asyncHelper->async();
//                    pRender->draw_frame(codecContext, waterFilterHelper->filterFrame,
//                                        pJobject1);
//                    av_frame_unref(waterFilterHelper->filterFrame);
                }
                //------------------

                av_frame_unref(pFrame);
            }
        }
        StartNextLoop:
        av_packet_unref(pPacket);
    }

    WholeLoopEnd:
    fclose(fp_yuv);
    waterFilterHelper->unInit();
    delete waterFilterHelper;
    waterFilterHelper = nullptr;
    delete asyncHelper;
    asyncHelper = nullptr;
    LOGCATE("loop end");

}

void PlayMp4Practice::stopPlay() {
    isLoop = false;
}

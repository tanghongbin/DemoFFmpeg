//
// Created by Admin on 2020/10/21.
//
#include <utils.h>
#include <CustomGLUtils.h>
#include <libavformat/avformat.h>

extern "C" {
#include <libavcodec/avcodec.h>
#include "FFmpegEncodeAudio.h"
}

void FFmpegEncodeAudio::init() {

}

void FFmpegEncodeAudio::encodeLoop() {
    AVCodec *codec;
    AVCodecContext *codecContext;
    int ret;
    const char * out_file_name = "/storage/emulated/0/ffmpegtest/encodeaudio.wav";
    FILE* outFile;

    outFile = fopen(out_file_name,"wb");
    if (!outFile) {
        LOGCATE("can't open file");
        return;
    }
    codec = avcodec_find_encoder(AV_CODEC_ID_AAC);
    if (!codec) {
        LOGCATE("can't find encoder");
        return;
    }
    codecContext = avcodec_alloc_context3(codec);
    if (!codecContext) {
        LOGCATE("can't alloc codecContext");
        return;
    }
    configAudioEncodeParams(codecContext);
    ret = avcodec_open2(codecContext,codec,NULL);
    if (checkNegativeReturn(ret,"can't open encoder")) return;

    AVFrame* frame = av_frame_alloc();
    if (!frame) {
        LOGCATE("can't alloc frame");
        return;
    }
    AVPacket* packet = av_packet_alloc();
    if (!packet) {
        LOGCATE("can't alloc packet");
        return;
    }
    configFrameParams(frame, codecContext);
    ret = av_frame_get_buffer(frame,0);
    if (checkNegativeReturn(ret,"can't alloc audio buffer")) return;

    // encode loop
    for(;;){

        ret = avcodec_send_frame(codecContext,frame);
        if (ret < 0){
            LOGCATE("audio record has end");
            break;
        }

        do{
            ret = avcodec_receive_packet(codecContext,packet);
            if (ret == AVERROR(EAGAIN) || ret == AVERROR_EOF){
                break;
            } else if (ret < 0){
                goto EndEncode;
            }
            fwrite(packet->data,1,packet->size,outFile);
            av_packet_unref(packet);
        } while (ret == 0);
    }

    EndEncode:
    av_packet_unref(packet);
    av_packet_free(&packet);
    av_frame_free(&frame);
    fclose(outFile);
    avcodec_free_context(&codecContext);

}

/**
 * 配置音频编码参数
 * @param pContext
 */
void FFmpegEncodeAudio::configAudioEncodeParams(AVCodecContext *context) {
    context->sample_rate = 44100;
    context->channel_layout = 16;
    context->channels = 1;
    context->bit_rate = 64000;
    context->sample_fmt = AV_SAMPLE_FMT_S16;
}

void FFmpegEncodeAudio::configFrameParams(AVFrame *pFrame, AVCodecContext *pContext) {
    pFrame->nb_samples = pContext->frame_size;
    pFrame->format = pContext->sample_fmt;
    pFrame->channel_layout = pContext->channel_layout;

}

//
// Created by Admin on 2020/10/21.
//
#include <utils.h>
#include <CustomGLUtils.h>
#include "FFmpegEncodeAudio.h"

extern "C" {
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libswresample/swresample.h>
#include <libavutil/opt.h>
}

using namespace std;

FFmpegEncodeAudio *FFmpegEncodeAudio::instance = nullptr;

FFmpegEncodeAudio *FFmpegEncodeAudio::getInstance() {
    if (instance == nullptr) {
        LOGCATE("create encode audio");
        instance = new FFmpegEncodeAudio;
    }
    return instance;
}

void FFmpegEncodeAudio::destroyInstance() {
    if (instance) {
        delete (instance);
        instance = nullptr;
    }
}

void FFmpegEncodeAudio::unInit() {
    int ret = av_write_trailer(ofmtCtx);
    if (ret < 0) {
        LOGCATE("av_write_trailer failed :%s",av_err2str(ret));
    }

    av_free(frame_buffer);
    av_packet_unref(packet);
    av_packet_free(&packet);
    av_frame_free(&frame);
    swr_free(&swr);
    if (ofmtCtx){
        avformat_free_context(ofmtCtx);
    }
    if (ofmtCtx && !( ofmtCtx->flags & AVFMT_NOFILE)){
        avio_close(ofmtCtx->pb);
    }
    if (ret < 0 && ret != AVERROR_EOF){
        remove(out_file_name);
    }
    avcodec_free_context(&codecContext);
}

void FFmpegEncodeAudio::init() {
    out_file_name =  getRandomStr("encodeaudio_",".aac","encodeAudios/");
    LOGCATE("has enter encode audio init function");

    ret = avformat_alloc_output_context2(&ofmtCtx,NULL,NULL,out_file_name);
    if (ret < 0){
        LOGCATE("avformat_alloc_output_context2 failed:%s",av_err2str(ret));
        return;
    }
    ofmt = ofmtCtx->oformat;
    codec = avcodec_find_encoder(ofmt->audio_codec);
//    codec = avcodec_find_encoder_by_name("libfdk_aac");
    if (!codec) {
        LOGCATE("can't find encoder");
        return;
    }
    LOGCATE("log coder:%d aac:%d",codec->id,AV_CODEC_ID_AAC);
    codecContext = avcodec_alloc_context3(codec);
    if (!codecContext) {
        LOGCATE("can't alloc codecContext");
        return;
    }
    configAudioEncodeParams(codecContext, codec);
    av_dump_format(ofmtCtx,0,out_file_name,1);
    ret = avcodec_open2(codecContext, codec, NULL);
    LOGCATE("check open audio encoder resultStr:%s",av_err2str(ret));
    if (checkNegativeReturn(ret, "can't open encoder")) return;

    // create output ctx
    oStream = avformat_new_stream(ofmtCtx,NULL);
    ret = avcodec_parameters_from_context(oStream->codecpar,codecContext);
    if (ret < 0){
        LOGCATE("avcodec_parameters_from_context failed:%s",av_err2str(ret));
        return;
    }
    oStream->codecpar->codec_tag = 0;
    if (!(ofmtCtx->flags & AVFMT_NOFILE)){
        if((ret = avio_open(&ofmtCtx->pb,out_file_name,AVIO_FLAG_WRITE)) < 0){
            LOGCATE("avio_open failed:%s",av_err2str(ret));
            return;
        }
    }

    frame = av_frame_alloc();
    if (!frame) {
        LOGCATE("can't alloc frame");
        return;
    }
    packet = av_packet_alloc();
    if (!packet) {
        LOGCATE("can't alloc packet");
        return;
    }
    configFrameParams(frame, codecContext);
    int size = av_samples_get_buffer_size(NULL, codecContext->channels, codecContext->frame_size,
                                          codecContext->sample_fmt, 1);
    frame_buffer = static_cast<uint8_t *>(av_malloc(size));
    avcodec_fill_audio_frame(frame, codecContext->channels, codecContext->sample_fmt,(const uint8_t*)frame_buffer, size, 1);
    if (checkNegativeReturn(ret, "can't alloc audio buffer")) return;

    ret = avformat_write_header(ofmtCtx,NULL);
    if (ret < 0) {
        LOGCATE("avformat_write_header failed :%s",av_err2str(ret));
        return;
    }

    //9.用于音频转码
    swr = swr_alloc();
    av_opt_set_channel_layout(swr, "in_channel_layout", AV_CH_LAYOUT_STEREO, 0);
    av_opt_set_channel_layout(swr, "out_channel_layout", AV_CH_LAYOUT_STEREO, 0);
    av_opt_set_int(swr, "in_sample_rate", 44100, 0);
    av_opt_set_int(swr, "out_sample_rate", 44100, 0);
    av_opt_set_sample_fmt(swr, "in_sample_fmt", AV_SAMPLE_FMT_S16, 0);
    av_opt_set_sample_fmt(swr, "out_sample_fmt", AV_SAMPLE_FMT_FLTP, 0);
    swr_init(swr);

    // encode loop


    LOGCATE("encode init finished");
}

int
FFmpegEncodeAudio::encodeAudioFrame(uint8_t *audio_buffer, int length) {
//    int size = av_samples_get_buffer_size(NULL, codecContext->channels, frame->nb_samples,
//                                          codecContext->sample_fmt, 1);

    int ret = 0;
    uint8_t *out[2];
    out[0] = new uint8_t[length];
    out[1] = new uint8_t[length];

    const uint8_t * convert_buffer = audio_buffer;
    ret = swr_convert(swr,out,length * 4,&convert_buffer,length/4);
    if (ret < 0){
        LOGCATE("convert error:%s",av_err2str(ret));
        goto EndEncode;
    }
    frame->data[0] = out[0];
    frame->data[1] = out[1];
    frame->pts = frame_index;

    ret = avcodec_send_frame(codecContext, frame);
    if (ret < 0) {
        LOGCATE("avcodec_send_frame failed :%s",av_err2str(ret));
        goto EndEncode;
    }
    while (ret >= 0) {
        ret = avcodec_receive_packet(codecContext, packet);
        if (ret == AVERROR(EAGAIN) || ret == AVERROR_EOF) {
            break;
        } else if (ret < 0) {
            LOGCATE("avcodec_receive_packet failed :%s",av_err2str(ret));
            goto EndEncode;
        }
        if (packet->pts == AV_NOPTS_VALUE){
            LOGCATE("no pts value");
        }
        packet->stream_index = oStream->index;

        ret = av_interleaved_write_frame(ofmtCtx,packet);

        if (ret < 0){
            LOGCATE("av_interleaved_write_frame failed :%s",av_err2str(ret));
            goto EndEncode;
        }
        av_packet_unref(packet);
        LOGCATE("encode success one frame:%d",frame_index);
        frame_index++;
    }

    EndEncode:
    av_packet_unref(packet);
    delete out[0];
    delete out[1];
    return ret;
}

/**
 * 配置音频编码参数
 * @param pContext
 */
void FFmpegEncodeAudio::configAudioEncodeParams(AVCodecContext *pContext, AVCodec *codec) {
    pContext->codec_id = codec->id;
    pContext->codec_type = AVMEDIA_TYPE_AUDIO;
    pContext->bit_rate = 96000;
//    AV_CODEC_ID_MP2
    // sample_fmt 二者选1
//    pContext->sample_fmt = AV_SAMPLE_FMT_S16; // 不支持
    pContext->sample_fmt = AV_SAMPLE_FMT_FLTP;
    if (!check_sample_fmt(codec,pContext->sample_fmt)){
        LOGCATE("this sample format is not support");
    }
    /* select other audio parameters supported by the encoder */
    pContext->sample_rate = select_sample_rate(codec);
    pContext->channel_layout = select_channel_layout(codec);
    pContext->channels = av_get_channel_layout_nb_channels(pContext->channel_layout);

    LOGCATE("look encode bit_rate:%jd sample_rate:%d  channel_layout:%jd channels:%d",
            pContext->bit_rate,
            pContext->sample_rate,
            pContext->channel_layout,
            pContext->channels);
}

int FFmpegEncodeAudio::check_sample_fmt(const AVCodec *codec, enum AVSampleFormat sample_fmt)
{
    const enum AVSampleFormat *p = codec->sample_fmts;

    while (*p != AV_SAMPLE_FMT_NONE) {
        if (*p == sample_fmt)
            return 1;
        p++;
    }
    return 0;
}

void FFmpegEncodeAudio::configFrameParams(AVFrame *pFrame, AVCodecContext *pContext) {
    pFrame->nb_samples = pContext->frame_size;
    pFrame->format = pContext->sample_fmt;
    pFrame->channel_layout = pContext->channel_layout;

}


int FFmpegEncodeAudio::select_sample_rate(AVCodec *codec) {
    const int *p;
    int best_samplerate = 0;

    if (!codec->supported_samplerates)
        return 44100;

    p = codec->supported_samplerates;
    while (*p) {
        if (!best_samplerate || abs(44100 - *p) < abs(44100 - best_samplerate))
            best_samplerate = *p;
        p++;
    }
    LOGCATE("pring best sample_rate:%d",best_samplerate);
    return best_samplerate;
}

uint64_t FFmpegEncodeAudio::select_channel_layout(AVCodec *codec) {
    const uint64_t *p;
    uint64_t best_ch_layout = 0;
    int best_nb_channels = 0;

    if (!codec->channel_layouts)
        return AV_CH_LAYOUT_STEREO;

    p = codec->channel_layouts;
    while (*p) {
        int nb_channels = av_get_channel_layout_nb_channels(*p);

        if (nb_channels > best_nb_channels) {
            best_ch_layout = *p;
            best_nb_channels = nb_channels;
        }
        p++;
    }
    return best_ch_layout;
}


int FFmpegEncodeAudio::check_sample_fmt(AVCodec *pCodec, AVSampleFormat sample_fmt) {
    const enum AVSampleFormat *p = pCodec->sample_fmts;

    while (*p != AV_SAMPLE_FMT_NONE) {
        if (*p == sample_fmt)
            return 1;
        p++;
    }
    return 0;
}

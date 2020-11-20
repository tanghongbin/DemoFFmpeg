//
// Created by Admin on 2020/10/21.
//
#include <utils.h>
#include <CustomGLUtils.h>
#include "FFmpegEncodeAudio.h"

extern "C" {
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
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

void FFmpegEncodeAudio::recordCallback(uint8_t *point, int sampleSize) {
    LOGCATE("audio record callback success :%p   size:%d", point, sampleSize);
    FFmpegEncodeAudio::getInstance()->audioFrameCopy(point, sampleSize);
}


void FFmpegEncodeAudio::audioFrameCopy(uint8_t *point, int sampleSize) {

//    LOGCATE("copy data from recorder to encoder");
}


void FFmpegEncodeAudio::init() {

    AVCodec *codec;
    AVCodecContext *codecContext;
    int ret;

    const char *out_file_name =  getRandomStr("encodeaudio_",".aac","encodeAudios/");
    const char *input_file_name = "/storage/emulated/0/ffmpegtest/capture.pcm";
    FILE *inFile;
    AVFrame *frame;
    AVPacket *packet;
    AVFormatContext* ofmtCtx;
    AVOutputFormat* ofmt;
    AVStream* oStream;
    uint8_t *frame_buffer;
    LOGCATE("has enter encode audio init function");
    inFile = fopen(input_file_name, "rb");
    if (!inFile) {
        LOGCATE("can't open input file");
        return;
    }

    ret = avformat_alloc_output_context2(&ofmtCtx,NULL,NULL,out_file_name);
    if (ret < 0){
        LOGCATE("avformat_alloc_output_context2 failed:%s",av_err2str(ret));
        return;
    }
    ofmt = ofmtCtx->oformat;
    codec = avcodec_find_encoder(ofmt->audio_codec);
    if (!codec) {
        LOGCATE("can't find encoder");
        return;
    }
    codecContext = avcodec_alloc_context3(codec);
    if (!codecContext) {
        LOGCATE("can't alloc codecContext");
        return;
    }
    configAudioEncodeParams(codecContext, codec);
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

    ret = encodeAudioLoop(codecContext, packet, frame, ofmtCtx, inFile, frame_buffer);
    // encode loop

    ret = av_write_trailer(ofmtCtx);
    if (ret < 0) {
        LOGCATE("av_write_trailer failed :%s",av_err2str(ret));
        return;
    }


    av_free(frame_buffer);
    av_packet_unref(packet);
    av_packet_free(&packet);
    av_frame_free(&frame);
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

    LOGCATE("encode finished");
}

int
FFmpegEncodeAudio::encodeAudioLoop(AVCodecContext *pContext, AVPacket *pPacket, AVFrame *pFrame,
                                   AVFormatContext *ofmtctx, FILE *inputFile, uint8_t *frame_buffer) {
    int size = av_samples_get_buffer_size(NULL, pContext->channels, pFrame->nb_samples,
                                          pContext->sample_fmt, 1);

    int ret = 0;
    int start, frameNum = INT_MAX;
    int packetCount = 0;
    for (start = 1; start < frameNum; start++) {
        if (fread(frame_buffer, 1, size, inputFile) <= 0) {
            LOGCATE("read has complete");
            break;
        } else if (ferror(inputFile)) {
            LOGCATE("read has error");
            ret = -1;
            break;
        }
        pFrame->data[0] = frame_buffer;
        pFrame->pts = start * 100;

//        LOGCATE("print after read_frame:%p",frame->data);
        ret = avcodec_send_frame(pContext, pFrame);
        if (ret < 0) {
            LOGCATE("avcodec_send_frame failed :%s",av_err2str(ret));
            break;
        }
        while (ret >= 0) {
            ret = avcodec_receive_packet(pContext, pPacket);
            if (ret == AVERROR(EAGAIN) || ret == AVERROR_EOF) {
                break;
            } else if (ret < 0) {
                LOGCATE("avcodec_receive_packet failed :%s",av_err2str(ret));
                goto EndEncode;
            }
            if (pPacket->pts == AV_NOPTS_VALUE){
                LOGCATE("no pts value");
            }
            pPacket->stream_index = 0;

            ret = av_interleaved_write_frame(ofmtctx,pPacket);

            if (ret < 0){
                LOGCATE("av_interleaved_write_frame failed :%s",av_err2str(ret));
                goto EndEncode;
            }
            av_packet_unref(pPacket);
            LOGCATE("encode success one frame,get a packet %d", packetCount++);
        }
    }

    EndEncode:
    av_packet_unref(pPacket);
    return ret;
}

/**
 * 配置音频编码参数
 * @param pContext
 */
void FFmpegEncodeAudio::configAudioEncodeParams(AVCodecContext *pContext, AVCodec *codec) {
    pContext->codec_id = codec->id;
    pContext->codec_type = AVMEDIA_TYPE_AUDIO;
    pContext->bit_rate = 64000;
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

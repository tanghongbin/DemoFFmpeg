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
    const char *out_file_name = "/storage/emulated/0/ffmpegtest/encodeaudio.aac";
    const char *input_file_name = "/storage/emulated/0/ffmpegtest/capture.pcm";
    FILE *outFile;
    FILE *inFile;
    AVFrame *frame;
    AVPacket *packet;
    LOGCATE("has enter encode audio init function");
    outFile = fopen(out_file_name, "wb");
    if (!outFile) {
        LOGCATE("can't open file");
        return;
    }
    inFile = fopen(input_file_name, "rb");
    if (!inFile) {
        LOGCATE("can't open input file");
        return;
    }
    codec = avcodec_find_encoder(AV_CODEC_ID_MP2);
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
    if (checkNegativeReturn(ret, "can't open encoder")) return;

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
    ret = av_frame_get_buffer(frame, 0);
    if (checkNegativeReturn(ret, "can't alloc audio buffer")) return;

    encodeAudioLoop(codecContext, packet, frame, outFile, inFile);
    // encode loop

    av_packet_unref(packet);
    av_packet_free(&packet);
    av_frame_free(&frame);
    fclose(outFile);
    avcodec_free_context(&codecContext);

    LOGCATE("encode finished");
}

void
FFmpegEncodeAudio::encodeAudioLoop(AVCodecContext *pContext, AVPacket *pPacket, AVFrame *pFrame,
                                   FILE *string1,
                                   FILE *inputFile) {
    int size = av_samples_get_buffer_size(NULL, pContext->channels, pFrame->nb_samples,
                                          pContext->sample_fmt, 1);
    uint8_t *frame_buffer = static_cast<uint8_t *>(av_malloc(size));
    int ret;
    int start, frameNum = 1000;
    int packetCount = 0;
    for (start = 0; start < frameNum; start++) {
        if (fread(frame_buffer, 1, size, inputFile) <= 0) {
            LOGCATE("read has complete");
            break;
        } else if (ferror(inputFile)) {
            break;
        }
        pFrame->data[0] = frame_buffer;
        pFrame->pts = start * 100;


//        LOGCATE("print after read_frame:%p",frame->data);
        ret = avcodec_send_frame(pContext, pFrame);
        if (ret < 0) {
            LOGCATE("audio record has end");
            break;
        }
        LOGCATE("has send frame success %d",start);

        while (ret >= 0) {
            ret = avcodec_receive_packet(pContext, pPacket);
            if (ret == AVERROR(EAGAIN) || ret == AVERROR_EOF) {
                break;
            } else if (ret < 0) {
                goto EndEncode;
            }
            fwrite(pPacket->data, 1, pPacket->size, string1);
            av_packet_unref(pPacket);
            LOGCATE("encode success one frame,get a packet %d",packetCount++);
        }
    }

    EndEncode:
    av_packet_unref(pPacket);
    av_free(frame_buffer);
}

/**
 * 配置音频编码参数
 * @param pContext
 */
void FFmpegEncodeAudio::configAudioEncodeParams(AVCodecContext *pContext, AVCodec *codec) {
    pContext->codec_id = codec->id;
    pContext->codec_type = AVMEDIA_TYPE_AUDIO;
    pContext->bit_rate = 64000;
    pContext->sample_fmt = AV_SAMPLE_FMT_S16;
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

void FFmpegEncodeAudio::initOffcialDemo() {
    AVCodec *codec;
    AVCodecContext *c = NULL;
    AVFrame *frame;
    AVPacket *pkt;
    int i, j, k, ret;
    FILE *f;
    uint16_t *samples;
    float t, tincr;

    const char *filename = "/storage/emulated/0/ffmpegtest/encodeaudio.aac";

    /* find the MP2 encoder */
    codec = avcodec_find_encoder(AV_CODEC_ID_MP2);
    if (!codec) {
        fprintf(stderr, "Codec not found\n");
        exit(1);
    }

    c = avcodec_alloc_context3(codec);
    if (!c) {
        fprintf(stderr, "Could not allocate audio codec context\n");
        exit(1);
    }

    /* put sample parameters */
    c->bit_rate = 64000;

    /* check that the encoder supports s16 pcm input */
    c->sample_fmt = AV_SAMPLE_FMT_S16;
    if (!check_sample_fmt(codec, c->sample_fmt)) {
        fprintf(stderr, "Encoder does not support sample format %s",
                av_get_sample_fmt_name(c->sample_fmt));
        exit(1);
    }

    /* select other audio parameters supported by the encoder */
    c->sample_rate = select_sample_rate(codec);
    c->channel_layout = select_channel_layout(codec);
    c->channels = av_get_channel_layout_nb_channels(c->channel_layout);

    /* open it */
    if (avcodec_open2(c, codec, NULL) < 0) {
        fprintf(stderr, "Could not open codec\n");
        exit(1);
    }

    f = fopen(filename, "wb");
    if (!f) {
        fprintf(stderr, "Could not open %s\n", filename);
        exit(1);
    }

    /* packet for holding encoded output */
    pkt = av_packet_alloc();
    if (!pkt) {
        fprintf(stderr, "could not allocate the packet\n");
        exit(1);
    }

    /* frame containing input raw audio */
    frame = av_frame_alloc();
    if (!frame) {
        fprintf(stderr, "Could not allocate audio frame\n");
        exit(1);
    }

    frame->nb_samples = c->frame_size;
    frame->format = c->sample_fmt;
    frame->channel_layout = c->channel_layout;

    /* allocate the data buffers */
    ret = av_frame_get_buffer(frame, 0);
    if (ret < 0) {
        fprintf(stderr, "Could not allocate audio data buffers\n");
        exit(1);
    }

    /* encode a single tone sound */
    t = 0;
    tincr = 2 * M_PI * 440.0 / c->sample_rate;
    for (i = 0; i < 200; i++) {
        /* make sure the frame is writable -- makes a copy if the encoder
         * kept a reference internally */
        ret = av_frame_make_writable(frame);
        if (ret < 0)
            exit(1);
        samples = (uint16_t *) frame->data[0];

        for (j = 0; j < c->frame_size; j++) {
            samples[2 * j] = (int) (sin(t) * 10000);

            for (k = 1; k < c->channels; k++)
                samples[2 * j + k] = samples[2 * j];
            t += tincr;
        }

        encodeAudioLoop(c, pkt, frame, f, nullptr);

    }

    /* flush the encoder */
    encodeAudioLoop(c, pkt, NULL, f, nullptr);

    fclose(f);

    av_frame_free(&frame);
    av_packet_free(&pkt);
    avcodec_free_context(&c);
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

//
// Created by Admin on 2020/10/21.
//

#ifndef DEMOC_FFMPEGENCODEAUDIO_H
#define DEMOC_FFMPEGENCODEAUDIO_H

#include <link.h>
#include <__mutex_base>


extern "C" {
#include <libswresample/swresample.h>
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
};

#define AAC_ENCODER_NAME "libfdk_aac"

using namespace std;

/**
 * 编码为单独文件，混合编码
 */
class FFmpegEncodeAudio {


public:

    static FFmpegEncodeAudio *getInstance();

    static void destroyInstance();

    void init();

    void unInit();

    int encodeAudioFrame(uint8_t *audio_buffer, int length);

private:

    static FFmpegEncodeAudio *instance;
    AVCodec *codec;
    AVCodecContext *codecContext;
    int ret;
    const char *out_file_name;
    AVFrame *frame;
    SwrContext *swr;
    AVPacket packet;
    AVFormatContext *ofmtCtx;
    AVOutputFormat *ofmt;
    AVStream *oStream;
    uint8_t *frame_buffer;

    void configAudioEncodeParams(AVCodecContext *pContext, AVCodec *pCodec);

    void configFrameParams(AVFrame *pFrame, AVCodecContext *pContext);

    int check_sample_fmt(const AVCodec *codec, enum AVSampleFormat sample_fmt);

    int select_sample_rate(AVCodec *pCodec);

    uint64_t select_channel_layout(AVCodec *pCodec);

    int check_sample_fmt(AVCodec *pCodec, AVSampleFormat format);
};


#endif //DEMOC_FFMPEGENCODEAUDIO_H

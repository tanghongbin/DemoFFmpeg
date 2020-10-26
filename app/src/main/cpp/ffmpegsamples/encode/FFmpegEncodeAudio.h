//
// Created by Admin on 2020/10/21.
//

#ifndef DEMOC_FFMPEGENCODEAUDIO_H
#define DEMOC_FFMPEGENCODEAUDIO_H

#include <link.h>
#include <__mutex_base>

extern "C" {
#include <libavcodec/avcodec.h>
};


using namespace std;

/**
 * 编码为单独文件，混合编码
 */
class FFmpegEncodeAudio {


public:

    static FFmpegEncodeAudio *getInstance();

    static void destroyInstance();

    void init();

    static void recordCallback(uint8_t *point, int sampleSize);

    void audioFrameCopy(uint8_t *frame, int sampleSize);

    void initOffcialDemo();

private:
    mutex mMutex;
    static FFmpegEncodeAudio *instance;


    void
    encodeAudioLoop(AVCodecContext *pContext, AVPacket *pPacket, AVFrame *pFrame, FILE *string1,
                    FILE *inputFile);

    void configAudioEncodeParams(AVCodecContext *pContext, AVCodec *pCodec);

    void configFrameParams(AVFrame *pFrame, AVCodecContext *pContext);


    int select_sample_rate(AVCodec *pCodec);

    uint64_t select_channel_layout(AVCodec *pCodec);


    int check_sample_fmt(AVCodec *pCodec, AVSampleFormat format);
};


#endif //DEMOC_FFMPEGENCODEAUDIO_H

//
// Created by Admin on 2020/9/16.
//

#ifndef DEMOC_AUDIORENDER_H
#define DEMOC_AUDIORENDER_H


extern "C" {
#include <libswresample/swresample.h>
};

#include <cstdint>
#include <string>
#include <SLES/OpenSLES.h>
#include <SLES/OpenSLES_Android.h>
#include "BaseRender.h"
#include "OpenSLESRender.h"
#include <thread>
#include <queue>

#define MAX_QUEUE_BUFFER_SIZE 3

// 音频编码采样率
static const int AUDIO_DST_SAMPLE_RATE = 44100;
// 音频编码通道数
static const int AUDIO_DST_CHANNEL_COUNTS = 2;
// 音频编码声道格式
static const uint64_t AUDIO_DST_CHANNEL_LAYOUT = AV_CH_LAYOUT_STEREO;
// 音频编码比特率
static const int AUDIO_DST_BIT_RATE = 64000;
// ACC音频一帧采样数
static const int ACC_NB_SAMPLES = 1024;

const AVSampleFormat DST_SAMPLT_FORMAT = AV_SAMPLE_FMT_S16;


class AudioRender : public BaseRender {

private:
    SwrContext *m_SwrContext;
    OpenSLESRender* openSlesRender;


public:
    void init(AVCodecContext *pContext, _jobject *instance, _jobject *pJobject);

    void draw_frame(AVCodecContext *pContext, AVFrame *pFrame,  _jobject *pJobject);

    void unInit();

    void eachPacket(AVPacket *packet, AVCodecContext *pContext);


};


#endif //DEMOC_AUDIORENDER_H

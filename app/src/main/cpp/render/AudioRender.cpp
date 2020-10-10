//
// Created by Admin on 2020/9/16.
//

extern "C" {
#include <libavutil/opt.h>
}

#include "AudioRender.h"
#include "CustomGLUtils.h"
#include <SLES/OpenSLES.h>
#include <SLES/OpenSLES_Android.h>

void AudioRender::init(AVCodecContext *codeCtx, _jobject *instance, _jobject *pJobject) {
    //1. 生成 resample 上下文，设置输入和输出的通道数、采样率以及采样格式，初始化上下文
    m_SwrContext = swr_alloc();

    av_opt_set_int(m_SwrContext, "in_channel_layout", codeCtx->channel_layout, 0);
    av_opt_set_int(m_SwrContext, "out_channel_layout", AUDIO_DST_CHANNEL_LAYOUT, 0);
    av_opt_set_int(m_SwrContext, "in_sample_rate", codeCtx->sample_rate, 0);
    av_opt_set_int(m_SwrContext, "out_sample_rate", AUDIO_DST_SAMPLE_RATE, 0);
    av_opt_set_sample_fmt(m_SwrContext, "in_sample_fmt", codeCtx->sample_fmt, 0);
    av_opt_set_sample_fmt(m_SwrContext, "out_sample_fmt", DST_SAMPLT_FORMAT, 0);
    swr_init(m_SwrContext);
    LOGCATE("AudioRender::Init");
    openSlesRender = new OpenSLESRender;
    openSlesRender->Init();
}


void AudioRender::unInit() {
    if (openSlesRender) {
        openSlesRender->UnInit();
        delete (openSlesRender);
        openSlesRender = nullptr;
    }
    if (m_SwrContext) {
        swr_free(&m_SwrContext);
        m_SwrContext = nullptr;
    }
}

void AudioRender::eachPacket(AVPacket *packet, AVCodecContext *pContext) {

    double bufferSizeEverySecond =
            2 * pContext->channels * pContext->sample_rate * pContext->sample_fmt;
    int64_t playTime =(int64_t)(packet->size / bufferSizeEverySecond * AV_TIME_BASE);
//    LOGCATE("每个packet的播放时间:%jd微妙  packet的时间戳:%jd",playTime,packet->pts);
    GLUtilC::master_audio_clock = packet->pts + playTime;
//    LOGCATE("每次音频的帧值:%jd",GLUtilC::master_audio_clock);
}

void AudioRender::draw_frame(AVCodecContext *pContext, AVFrame *pFrame,
                             _jobject *pJobject) {

    // 2.申请输出buffer
    int m_nbSamples = (int) av_rescale_rnd(ACC_NB_SAMPLES, AUDIO_DST_SAMPLE_RATE,
                                           pContext->sample_rate, AV_ROUND_UP);

    int m_BufferSize = av_samples_get_buffer_size(NULL, AUDIO_DST_CHANNEL_COUNTS, m_nbSamples,
                                                  DST_SAMPLT_FORMAT, 1);
    uint8_t *m_AudioOutBuffer = (uint8_t *) malloc(m_BufferSize);


    //3. 重采样，frame 为解码帧
    int result = swr_convert(m_SwrContext, &m_AudioOutBuffer, m_BufferSize / 2,
                             (const uint8_t **) pFrame->data, pFrame->nb_samples);
//    LOGCATE("resample success result:%d",result);
//    LOGCATE("当前音频时间戳:%jd", GLUtilC::master_audio_clock);
    if (result > 0) {
        openSlesRender->RenderAudioFrame(m_AudioOutBuffer, m_BufferSize);
    }

//4. 释放资源
    if (m_AudioOutBuffer) {
        free(m_AudioOutBuffer);
        m_AudioOutBuffer = nullptr;
    }

}


#include <render/AudioDataConverter.h>
#include <media/OpenSLESRender.h>
#include <utils/utils.h>


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

//
// Created by Admin on 2021/5/24.
//
void AudioDataConverter::Init(AVCodecContext* codeCtx){

    m_SwrContext = swr_alloc();
    av_opt_set_int(m_SwrContext, "in_channel_layout", codeCtx->channel_layout, 0);
    av_opt_set_int(m_SwrContext, "out_channel_layout", AUDIO_DST_CHANNEL_LAYOUT, 0);
    av_opt_set_int(m_SwrContext, "in_sample_rate", codeCtx->sample_rate, 0);
    av_opt_set_int(m_SwrContext, "out_sample_rate", AUDIO_DST_SAMPLE_RATE, 0);
    av_opt_set_sample_fmt(m_SwrContext, "in_sample_fmt", codeCtx->sample_fmt, 0);
    av_opt_set_sample_fmt(m_SwrContext, "out_sample_fmt", DST_SAMPLT_FORMAT, 0);
    swr_init(m_SwrContext);

    LOGCATE("log sample_rate ------%d",codeCtx->sample_rate);

    // 2.申请输出buffer
    int m_nbSamples = (int) av_rescale_rnd(ACC_NB_SAMPLES, AUDIO_DST_SAMPLE_RATE,
                                           codeCtx->sample_rate, AV_ROUND_UP);
    m_BufferSize = av_samples_get_buffer_size(NULL, AUDIO_DST_CHANNEL_COUNTS, m_nbSamples,
                                              DST_SAMPLT_FORMAT, 1);
    m_AudioOutBuffer = (uint8_t *) malloc(m_BufferSize);
    openSlesRender = new OpenSLESRender;
    openSlesRender->Init();
}

void AudioDataConverter::covertData(AVFrame * pFrame){
    //3. 重采样，frame 为解码帧
    int result = swr_convert(m_SwrContext, &m_AudioOutBuffer, m_BufferSize / 2,
                             (const uint8_t **) pFrame->data, pFrame->nb_samples);
//    LOGCATE("resample result: %d",result);
//    LOGCATE("当前音频时间戳:%jd", GLUtilC::master_audio_clock);
    if (result > 0) {
        openSlesRender->RenderAudioFrame(m_AudioOutBuffer, m_BufferSize);
    }

}

void AudioDataConverter::Destroy(){
    free(m_AudioOutBuffer);
    openSlesRender->UnInit();
    delete openSlesRender;
}
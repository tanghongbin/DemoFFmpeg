//
// Created by Admin on 2021/5/24.
//

#include <decoder/ImlFFmpegDecoder.h>
#include <render/FFmpegVideoToRgbConverter.h>
#include <render/FFmpegVideoConverter.h>


AudioFFmpegDecoder* AudioFFmpegDecoder::instance = nullptr;

BaseDataCoverter *AudioFFmpegDecoder::createConverter()  {
    return new FFmpegAudioConverter;
}

AudioFFmpegDecoder* AudioFFmpegDecoder::getInstance(){
    if (instance == nullptr){
        instance = new AudioFFmpegDecoder;
    }
    return instance;
}

void AudioFFmpegDecoder::destroyInstance(){
    if (instance != nullptr){
        delete instance;
        instance = nullptr;
    }
}

BaseDataCoverter *VideoFFmpegDecoder::createConverter()  {
    FFmpegVideoConverter* glfFmpegConverter = new FFmpegVideoConverter;
    return new FFmpegVideoConverter;
}

void VideoFFmpegDecoder::drawVideoFrame()  {
    if (videoRender) videoRender->DrawFrame();
}

void VideoFFmpegDecoder::OnSurfaceChanged(int oreration, int windowW, int windowH){
    windowWidth = windowW;
    windowHeight = windowH;
    mOreration = oreration;
    OnSizeReady();
}

VideoFFmpegDecoder::VideoFFmpegDecoder()  {
    videoRender = 0;
    codeCtx = 0;
}

void VideoFFmpegDecoder::OnSizeReady() {
    if (codeCtx == nullptr || videoRender == nullptr) return;
    int m_VideoWidth = codeCtx->width;
    int m_VideoHeight = codeCtx->height;
    int m_RenderWidth,m_RenderHeight;
    setupRenderDimension(mOreration,windowWidth,windowHeight,m_VideoWidth,m_VideoHeight,&m_RenderWidth,&m_RenderHeight);
    auto * render = dynamic_cast<VideoRender *>(videoRender);
    render->OnRenderSizeChanged(windowWidth,windowHeight,m_RenderWidth,m_RenderHeight);
}



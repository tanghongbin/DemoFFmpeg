//
// Created by Admin on 2021/5/24.
//

#include <decoder/ImlDecoder.h>
#include <render/VideoDataConverter.h>
#include <render/OpenGLFFmpegConverter.h>


AudioDecoder* AudioDecoder::instance = nullptr;

BaseDataCoverter *AudioDecoder::createConverter()  {
    return new AudioDataConverter;
}

AudioDecoder* AudioDecoder::getInstance(){
    if (instance == nullptr){
        instance = new AudioDecoder;
    }
    return instance;
}

void AudioDecoder::destroyInstance(){
    if (instance != nullptr){
        delete instance;
        instance = nullptr;
    }
}

BaseDataCoverter *VideoDecoder::createConverter()  {
    OpenGLFFmpegConverter* glfFmpegConverter = new OpenGLFFmpegConverter;
    return new OpenGLFFmpegConverter;
}

void VideoDecoder::drawVideoFrame()  {
    if (videoRender) videoRender->DrawFrame();
}

void VideoDecoder::OnSurfaceChanged(int oreration,int windowW,int windowH){
    windowWidth = windowW;
    windowHeight = windowH;
    mOreration = oreration;
    OnSizeReady();
}

VideoDecoder::VideoDecoder()  {
    videoRender = 0;
    codeCtx = 0;
}

void VideoDecoder::OnSizeReady() {
    if (codeCtx == nullptr || videoRender == nullptr) return;
    int m_VideoWidth = codeCtx->width;
    int m_VideoHeight = codeCtx->height;
    int m_RenderWidth,m_RenderHeight;
    setupRenderDimension(mOreration,windowWidth,windowHeight,m_VideoWidth,m_VideoHeight,&m_RenderWidth,&m_RenderHeight);
    videoRender->OnRenderSizeChanged(windowWidth,windowHeight,m_RenderWidth,m_RenderHeight);
}



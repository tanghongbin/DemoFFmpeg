//
// Created by Admin on 2021/5/24.
//

#include <decoder/ImlDecoder.h>
#include <render/VideoDataConverter.h>
#include <render/OpenGLFFmpegConverter.h>


BaseDataCoverter *AudioDecoder::createConverter()  {
    return new AudioDataConverter;
}


BaseDataCoverter *VideoDecoder::createConverter()  {
    OpenGLFFmpegConverter* glfFmpegConverter = new OpenGLFFmpegConverter;
    return new OpenGLFFmpegConverter;
}

void VideoDecoder::drawVideoFrame()  {
    if (videoRender) videoRender->DrawFrame();
}

VideoDecoder::VideoDecoder()  {
    videoRender = 0;
}



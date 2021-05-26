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
    return new OpenGLFFmpegConverter;
}

void VideoDecoder::drawVideoFrame()  {
    if (mDataConverter) mDataConverter->drawVideoFrame();
}

VideoDecoder::VideoDecoder()  {
    videoRender = 0;
}



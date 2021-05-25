//
// Created by Admin on 2021/5/24.
//

#include <decoder/ImlDecoder.h>
#include <render/VideoDataConverter.h>


BaseDataCoverter *AudioDecoder::createConverter()  {
    return new AudioDataConverter;
}


BaseDataCoverter *VideoDecoder::createConverter()  {
    return new VideoDataConverter;
}

void VideoDecoder::drawVideoFrame()  {
    if (mDataConverter) mDataConverter->drawVideoFrame();
}



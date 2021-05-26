//
// Created by Admin on 2021/5/24.
//

#ifndef DEMOFFMPEG_IMLDECODER_H
#define DEMOFFMPEG_IMLDECODER_H

#include "BaseDecoder.h"
#include <cstdint>
#include <render/VideoRender.h>

class AudioDecoder : public BaseDecoder{
    BaseDataCoverter * createConverter();
};

class VideoDecoder : public BaseDecoder{
protected:
    BaseDataCoverter * createConverter();
public:
    VideoDecoder();
    void drawVideoFrame();
};

#endif //DEMOFFMPEG_IMLDECODER_H

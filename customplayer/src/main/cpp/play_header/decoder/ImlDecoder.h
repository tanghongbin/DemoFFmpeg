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
    int windowWidth;
    int windowHeight;
    // 1-竖屏，2-横屏
    int mOreration;
public:
    VideoDecoder();
    void drawVideoFrame();
    void OnSurfaceChanged(int oreration,int width,int height);
    void OnSizeReady();
};

#endif //DEMOFFMPEG_IMLDECODER_H

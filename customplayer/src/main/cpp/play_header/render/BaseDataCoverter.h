//
// Created by Admin on 2021/5/24.
//

#ifndef DEMOFFMPEG_BASEDATACOVERTER_H
#define DEMOFFMPEG_BASEDATACOVERTER_H

#include <cstdint>
#include "VideoRender.h"

extern "C" {
#include <libavcodec/avcodec.h>
#include <libavutil/frame.h>
};

// 数据转换，yuv -> rgb,音频重采样等
class BaseDataCoverter{
public:
    VideoRender* videoRender = 0;
    virtual void Init(AVCodecContext* codecContext) = 0;
    virtual void covertData(AVFrame *data) = 0;
    virtual void Destroy() = 0;
    virtual void drawVideoFrame() {};
};
#endif //DEMOFFMPEG_BASEDATACOVERTER_H

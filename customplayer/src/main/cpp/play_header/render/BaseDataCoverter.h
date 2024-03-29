//
// Created by Admin on 2021/5/24.
//

#ifndef DEMOFFMPEG_BASEDATACOVERTER_H
#define DEMOFFMPEG_BASEDATACOVERTER_H

#include <cstdint>
#include "VideoRender.h"
#include "utils/utils.h"

extern "C" {
#include <libavcodec/avcodec.h>
#include <libavutil/frame.h>
};

typedef struct _data_convert_info {
    uint8_t * data;
    int size;
    AVFrame * frameData;
} DataConvertInfo;

// 数据转换，yuv -> rgb,音频重采样等
class BaseDataCoverter{
protected:
    bool isDestroyed;
public:
    ConvertResult convertResult;
    void * baseDecoder;
    virtual void Init(AVCodecContext* codecContext) = 0;
    virtual void covertData(DataConvertInfo *data) = 0;
    virtual void Destroy() = 0;
    virtual void drawVideoFrame() {};
    BaseDataCoverter(){
        isDestroyed = false;
    }
};
#endif //DEMOFFMPEG_BASEDATACOVERTER_H

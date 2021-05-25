//
// Created by Admin on 2021/5/24.
//

#ifndef DEMOFFMPEG_VIDEODATACONVERTER_H
#define DEMOFFMPEG_VIDEODATACONVERTER_H

extern "C" {
#include "../../include/libavcodec/avcodec.h"
#include "../../include/libavutil/frame.h"
#include <libswscale/swscale.h>
#include <libavutil/imgutils.h>
}

#include "BaseDataCoverter.h"
#include "VideoRender.h"


class VideoDataConverter : public BaseDataCoverter{
private:
    int targetWidth,targetHeight;
    uint8_t * targetData;
    AVFrame* targetFrame;
    VideoRender* videoRender;
public:
    void Init(AVCodecContext* codecContext) ;
    void Destroy() ;
    void covertData(AVFrame* data);
    void drawVideoFrame();
    void InitVideoRender();

    SwsContext *swsCtx;
};

#endif //DEMOFFMPEG_VIDEODATACONVERTER_H

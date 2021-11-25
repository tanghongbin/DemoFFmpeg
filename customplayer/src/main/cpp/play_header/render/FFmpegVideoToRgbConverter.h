//
// Created by Admin on 2021/5/24.
//

#ifndef DEMOFFMPEG_FFMPEGVIDEOTORGBCONVERTER_H
#define DEMOFFMPEG_FFMPEGVIDEOTORGBCONVERTER_H

extern "C" {
#include "../../include/libavcodec/avcodec.h"
#include "../../include/libavutil/frame.h"
#include <libswscale/swscale.h>
#include <libavutil/imgutils.h>
}

#include "BaseDataCoverter.h"
#include "VideoRender.h"


/***
 * ffmpeg video 转rgb
 */
class FFmpegVideoToRgbConverter : public BaseDataCoverter{
private:
    int targetWidth,targetHeight,videoWidth,videoHeight;
    uint8_t * targetData;
    AVFrame* targetFrame;
public:
    void Init(AVCodecContext* codecContext) ;
    void Destroy() ;
    void covertData(DataConvertInfo * data);
    void drawVideoFrame();
    SwsContext *swsCtx;
};

#endif //DEMOFFMPEG_FFMPEGVIDEOTORGBCONVERTER_H

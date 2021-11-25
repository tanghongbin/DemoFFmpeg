//
// Created by Admin on 2020/10/12.
//

extern "C" {
#include <libswscale/swscale.h>
#include <libavutil/imgutils.h>
}

#include "../../play_header/render/FFmpegVideoConverter.h"


void FFmpegVideoConverter::Init(AVCodecContext *codeCtx) {

}


void FFmpegVideoConverter::Destroy() {
    isDestroyed = true;
}

void FFmpegVideoConverter::covertData(DataConvertInfo *data) {
    if (isDestroyed) return;
    AVFrame *frame = data->frameData;
    int currentPixfmt = frame->format;
    NativeOpenGLImage image;
//    LOGCATE("print format :%d",currentPixfmt);
    if (currentPixfmt == AV_PIX_FMT_YUV420P || currentPixfmt == AV_PIX_FMT_YUVJ420P) {
        image.format = IMAGE_FORMAT_I420;
        image.width = frame->width;
        image.height = frame->height;
        image.pLineSize[0] = frame->linesize[0];
        image.pLineSize[1] = frame->linesize[1];
        image.pLineSize[2] = frame->linesize[2];
        image.ppPlane[0] = frame->data[0];
        image.ppPlane[1] = frame->data[1];
        image.ppPlane[2] = frame->data[2];
        if (frame->data[0] && frame->data[1] && !frame->data[2] &&
            frame->linesize[0] == frame->linesize[1] && frame->linesize[2] == 0) {
            // on some android device, output of h264 mediacodec decoder is NV12 兼容某些设备可能出现的格式不匹配问题
            image.format = IMAGE_FORMAT_NV12;
        }
    } else if (currentPixfmt == AV_PIX_FMT_NV12) {
        image.format = IMAGE_FORMAT_NV12;
        image.width = frame->width;
        image.height = frame->height;
        image.pLineSize[0] = frame->linesize[0];
        image.pLineSize[1] = frame->linesize[1];
        image.ppPlane[0] = frame->data[0];
        image.ppPlane[1] = frame->data[1];
    } else if (currentPixfmt == AV_PIX_FMT_NV21) {
        image.format = IMAGE_FORMAT_NV21;
        image.width = frame->width;
        image.height = frame->height;
        image.pLineSize[0] = frame->linesize[0];
        image.pLineSize[1] = frame->linesize[1];
        image.ppPlane[0] = frame->data[0];
        image.ppPlane[1] = frame->data[1];
    } else if (currentPixfmt == AV_PIX_FMT_RGBA) {
        image.format = IMAGE_FORMAT_RGBA;
        image.width = frame->width;
        image.height = frame->height;
        image.pLineSize[0] = frame->linesize[0];
        image.ppPlane[0] = frame->data[0];
    } else {

//        sws_scale(m_SwsContext, frame->data, frame->linesize, 0,
//                  m_VideoHeight, m_RGBAFrame->data, m_RGBAFrame->linesize);
//        image.format = IMAGE_FORMAT_RGBA;
//        image.width = m_RenderWidth;
//        image.height = m_RenderHeight;
//        image.ppPlane[0] = m_RGBAFrame->data[0];
    }
//    LOGCATE("decode success");
    convertResult(baseDecoder,&image,0);
}

//
// Created by Admin on 2020/10/12.
//

extern "C" {
#include <libswscale/swscale.h>
#include <libavutil/imgutils.h>
}

#include <OpenGLImageDef.h>
#include "OpenGLFFmpegDecoder.h"
#include "OpenGLFFmpegRender.h"
#include <VideoGLRender.h>


void OpenGLFFmpegDecoder::init(AVCodecContext *codeCtx, _jobject *instance, _jobject *pJobject) {

    int windowWidth = OpenGLFFmpegRender::mWindowWidth;
    int windowHeight = OpenGLFFmpegRender::mWindowHeight;

    if (m_VideoWidth > m_VideoHeight) {
        if (windowWidth < windowHeight * m_VideoWidth / m_VideoHeight) {
            m_RenderWidth = windowWidth;
            m_RenderHeight = windowWidth * m_VideoHeight / m_VideoWidth;
        } else {
            m_RenderWidth = windowHeight * m_VideoWidth / m_VideoHeight;
            m_RenderHeight = windowHeight;
        }
    } else {
        m_RenderWidth = m_VideoHeight;
        m_RenderHeight = m_VideoWidth;
    }

    //2. 获取转换的上下文
    m_SwsContext = sws_getContext(m_VideoWidth, m_VideoHeight,
                                  codeCtx->pix_fmt,
                                  m_RenderWidth, m_RenderHeight, AV_PIX_FMT_RGBA,
                                  SWS_FAST_BILINEAR, NULL, NULL, NULL);


    m_RGBAFrame = av_frame_alloc();
//计算 Buffer 的大小
    int bufferSize = av_image_get_buffer_size(AV_PIX_FMT_RGBA, m_VideoWidth, m_VideoHeight, 1);
    m_FrameBuffer = (uint8_t *) av_malloc(bufferSize * sizeof(uint8_t));

    av_image_fill_arrays(m_RGBAFrame->data, m_RGBAFrame->linesize, m_FrameBuffer, AV_PIX_FMT_RGBA,
                         m_VideoWidth, m_VideoHeight, 1);




}


void OpenGLFFmpegDecoder::unInit() {
    if (m_SwsContext != nullptr) {
        sws_freeContext(m_SwsContext);
        m_SwsContext = nullptr;
    }

    //4. 释放资源
    if (m_RGBAFrame != nullptr) {
        av_frame_free(&m_RGBAFrame);
        m_RGBAFrame = nullptr;
    }

    if (m_FrameBuffer != nullptr) {
        free(m_FrameBuffer);
        m_FrameBuffer = nullptr;
    }
}

void OpenGLFFmpegDecoder::eachPacket(AVPacket *packet, AVCodecContext *pContext) {

}


void OpenGLFFmpegDecoder::draw_frame(AVCodecContext *mCodeCtx, AVFrame *frame,
                                     _jobject *pJobject) {
    NativeOpenGLImage image;
//    LOGCATE("print format :%d",mCodeCtx->pix_fmt);
    if (mCodeCtx->pix_fmt == AV_PIX_FMT_YUV420P || mCodeCtx->pix_fmt == AV_PIX_FMT_YUVJ420P) {
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
    } else if (mCodeCtx->pix_fmt == AV_PIX_FMT_NV12) {
        image.format = IMAGE_FORMAT_NV12;
        image.width = frame->width;
        image.height = frame->height;
        image.pLineSize[0] = frame->linesize[0];
        image.pLineSize[1] = frame->linesize[1];
        image.ppPlane[0] = frame->data[0];
        image.ppPlane[1] = frame->data[1];
    } else if (mCodeCtx->pix_fmt == AV_PIX_FMT_NV21) {
        image.format = IMAGE_FORMAT_NV21;
        image.width = frame->width;
        image.height = frame->height;
        image.pLineSize[0] = frame->linesize[0];
        image.pLineSize[1] = frame->linesize[1];
        image.ppPlane[0] = frame->data[0];
        image.ppPlane[1] = frame->data[1];
    } else if (mCodeCtx->pix_fmt == AV_PIX_FMT_RGBA) {
        image.format = IMAGE_FORMAT_RGBA;
        image.width = frame->width;
        image.height = frame->height;
        image.pLineSize[0] = frame->linesize[0];
        image.ppPlane[0] = frame->data[0];
    } else {

        sws_scale(m_SwsContext, frame->data, frame->linesize, 0,
                  m_VideoHeight, m_RGBAFrame->data, m_RGBAFrame->linesize);
        image.format = IMAGE_FORMAT_RGBA;
        image.width = m_RenderWidth;
        image.height = m_RenderHeight;
        image.ppPlane[0] = m_RGBAFrame->data[0];
    }
//    LOGCATE("decode success");
    VideoGLRender::GetInstance()->RenderVideoFrame(&image);
}
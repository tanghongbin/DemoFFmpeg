//
// Created by Admin on 2020/9/27.
//

#include <utils.h>
#include <ImageDef.h>
#include "YuvToImageRender.h"
#include "OpenGLFFmpegRender.h"

extern "C" {
#include <libavutil/imgutils.h>
}

int YuvToImageRender::mWindowWidth = 0;
int YuvToImageRender::mWindowHeight = 0;

void YuvToImageRender::init(AVCodecContext *pContext, _jobject *instance, _jobject *pJobject) {
    mCodeCtx = pContext;
    LOGCATE("prepare draw each frame");
    m_VideoWidth = pContext->width;
    m_VideoHeight = pContext->height;
    LOGCATE("video width:%d  height:%d", m_VideoWidth, m_VideoHeight);

    int windowWidth = mWindowWidth;
    int windowHeight = mWindowHeight;

    if (windowWidth < windowHeight * m_VideoWidth / m_VideoHeight) {
        m_RenderWidth = windowWidth;
        m_RenderHeight = windowWidth * m_VideoHeight / m_VideoWidth;
    } else {
        m_RenderWidth = windowHeight * m_VideoWidth / m_VideoHeight;
        m_RenderHeight = windowHeight;
    }

    //2. 获取转换的上下文
    m_SwsContext = sws_getContext(m_VideoWidth, m_VideoHeight,
                                  pContext->pix_fmt,
                                  m_RenderWidth, m_RenderHeight, AV_PIX_FMT_RGBA,
                                  SWS_FAST_BILINEAR, NULL, NULL, NULL);


    m_RGBAFrame = av_frame_alloc();
//计算 Buffer 的大小
    int bufferSize = av_image_get_buffer_size(AV_PIX_FMT_RGBA, m_VideoWidth, m_VideoHeight, 1);
    m_FrameBuffer = (uint8_t *) av_malloc(bufferSize * sizeof(uint8_t));

    av_image_fill_arrays(m_RGBAFrame->data, m_RGBAFrame->linesize, m_FrameBuffer, AV_PIX_FMT_RGBA,
                         m_VideoWidth, m_VideoHeight, 1);

    LOGCATE("NativeRender::Init window[w,h]=[%d, %d],DstSize[w, h]=[%d, %d]", windowWidth, windowHeight, m_RenderWidth, m_RenderHeight);

    mRenderInterface = OpenGLFFmpegRender::getInstance();
    OpenGLFFmpegRender::getInstance()->init(windowWidth,windowHeight);
}

void YuvToImageRender::unInit() {
    OpenGLFFmpegRender::getInstance()->unInit();
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
    if (mRenderInterface != nullptr){
        mRenderInterface = nullptr;
    }
}

void YuvToImageRender::draw_frame(AVCodecContext *pContext, AVFrame *frame,
                             _jobject *pJobject)  {

    NativeImage image;

    sws_scale(m_SwsContext, frame->data, frame->linesize, 0,
              m_VideoHeight, m_RGBAFrame->data, m_RGBAFrame->linesize);

    image.format = IMAGE_FORMAT_RGBA;
    image.width = m_RenderWidth;
    image.height = m_RenderHeight;
    image.ppPlane[0] = m_RGBAFrame->data[0];

    LOGCATE("already prepare data ,it's ok to draw videoHeight:%d",m_VideoHeight);
    if (mRenderInterface)
        mRenderInterface->render_video(&image);
    return;
    if(mCodeCtx->pix_fmt == AV_PIX_FMT_YUV420P || mCodeCtx->pix_fmt == AV_PIX_FMT_YUVJ420P) {
        image.format = IMAGE_FORMAT_I420;
        image.width = frame->width;
        image.height = frame->height;
        image.pLineSize[0] = frame->linesize[0];
        image.pLineSize[1] = frame->linesize[1];
        image.pLineSize[2] = frame->linesize[2];
        image.ppPlane[0] = frame->data[0];
        image.ppPlane[1] = frame->data[1];
        image.ppPlane[2] = frame->data[2];
        if(frame->data[0] && frame->data[1] && !frame->data[2] && frame->linesize[0] == frame->linesize[1] && frame->linesize[2] == 0) {
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
    LOGCATE("already prepare data ,it's ok to draw");
    if (mRenderInterface)
    mRenderInterface->render_video(&image);
}



void YuvToImageRender::eachPacket(AVPacket *packet, AVCodecContext *pContext) {

}
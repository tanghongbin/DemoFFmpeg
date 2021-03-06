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

int OpenGLFFmpegDecoder::m_RenderWidth = 0;
int OpenGLFFmpegDecoder::m_RenderHeight = 0;

void OpenGLFFmpegDecoder::init(AVCodecContext *codeCtx, _jobject *instance, _jobject *pJobject) {

    vec2 &dimesion = VideoGLRender::GetInstance()->m_ScreenSize;
    // todo 暂时写死
    int windowWidth = 2159;
    int windowHeight = 1080;
    m_VideoWidth = codeCtx->width;
    m_VideoHeight = codeCtx->height;

    setupRenderDimension(windowWidth,windowHeight,m_VideoWidth,m_VideoHeight,&m_RenderWidth,&m_RenderHeight);

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

    class CallA : public MsgCallback {
    public:
        void call(JNIEnv* jni,jobject obj,jmethodID methodId){
            LOGCATE("call has called child jni:%p obj:%p methodId:%p",&jni,obj,methodId);
            jni->CallVoidMethod(obj,methodId,m_RenderWidth,m_RenderHeight);
        }
    } callback;
    sendMsgWithCallback(instance, RENDER_DIMENSION_CALLBACK, "(II)V",(MsgCallback*)&callback);

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
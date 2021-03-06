//
// Created by Admin on 2020/9/16.
//

extern "C" {
#include <libavutil/imgutils.h>
#include <libavformat/avformat.h>
}

#include <unistd.h>
#include <ImageDef.h>
#include <time.h>
#include <CustomGLUtils.h>
#include <helpers/JavaVmManager.h>
#include "VideoRender.h"
#include "utils.h"
#include "Mp4Demo.h"
#include "utils.h"


int VideoRender::m_RenderWidth = 0;
int VideoRender::m_RenderHeight = 0;
int64_t VideoRender::duration = 0;


void VideoRender::init(AVCodecContext *pContext, _jobject *instance, _jobject *pJobject) {

    LOGCATE("prepare draw each frame");
    m_VideoWidth = pContext->width;
    m_VideoHeight = pContext->height;
    LOGCATE("video width:%d  height:%d codeWidth:%d codeHeight:%d", m_VideoWidth, m_VideoHeight,
            pContext->coded_width, pContext->coded_height);

    bool isAttach = false;
    JNIEnv *jniEnv = JavaVmManager::GetEnv(&isAttach);
    //1. 利用 Java 层 SurfaceView 传下来的 Surface 对象，获取 ANativeWindow
    m_NativeWindow = ANativeWindow_fromSurface(jniEnv, pJobject);
    if (isAttach) JavaVmManager::detachCurrentThread();

    int windowWidth = ANativeWindow_getWidth(m_NativeWindow);
    int windowHeight = ANativeWindow_getHeight(m_NativeWindow);

    setupRenderDimension(windowWidth, windowHeight, m_VideoWidth, m_VideoHeight, &m_RenderWidth,
                         &m_RenderHeight);

    LOGCATE("cameraWidth:%d windowHeight:%d \nvideoWidth:%d videoHeight:%d \n renderWidth:%d renderHeight:%d",
            windowWidth, windowHeight, m_VideoWidth, m_VideoHeight, m_RenderWidth, m_RenderHeight);
    //2. 获取转换的上下文
    m_SwsContext = sws_getContext(m_VideoWidth, m_VideoHeight,
                                  pContext->pix_fmt,
                                  m_RenderWidth, m_RenderHeight, AV_PIX_FMT_RGBA,
                                  SWS_FAST_BILINEAR, NULL, NULL, NULL);

    //2. 设置渲染区域和输入格式
    ANativeWindow_setBuffersGeometry(m_NativeWindow, m_RenderWidth,
                                     m_RenderHeight, WINDOW_FORMAT_RGBA_8888);

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

    LOGCATE("NativeRender::Init window[w,h]=[%d, %d],DstSize[w, h]=[%d, %d]", windowWidth,
            windowHeight, m_RenderWidth, m_RenderHeight);

}

void VideoRender::unInit() {
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

    //4. 释放 ANativeWindow
    if (m_NativeWindow)
        ANativeWindow_release(m_NativeWindow);

}

void VideoRender::draw_frame(AVCodecContext *pContext, AVFrame *pFrame,
                             _jobject *pJobject) {

    // 如果当前视频帧不符合要求，直接返回
//    if (!checkFrameValid(pFrame)){
////        LOGCATE("当前视频镇不合格:%d",pFrame->key_frame);
//        return;
//    }
    long long cur = GetSysCurrentTime();
    if (mLastTime == 0) {
        mLastTime = cur;
    } else {
//        LOGCATE("draw frame spend time:%jd",cur - mLastTime);
        mLastTime = cur;
    }
//3. 格式转换
    sws_scale(m_SwsContext, pFrame->data, pFrame->linesize, 0, m_VideoHeight, m_RGBAFrame->data,
              m_RGBAFrame->linesize);
    displayToSurface(m_RGBAFrame);
//    LOGCATE("log every frame timestamp:%jd",(pFrame->pts));
//    LOGCATE("log every frame best timestamp:%jd",(pFrame->best_effort_timestamp));

//    usleep(16 * 1000);
}

void VideoRender::displayToSurface(AVFrame *pFrame) {

    time_t last = lastTime;

//    LOGCATE("display every frame: %d",time(&lastTime) - last);


//锁定当前 Window ，获取屏幕缓冲区 Buffer 的指针
    ANativeWindow_lock(m_NativeWindow, &m_NativeWindowBuffer, nullptr);
    uint8_t *dstBuffer = static_cast<uint8_t *>(m_NativeWindowBuffer.bits);

    int srcLineSize = pFrame->linesize[0];//输入图的步长（一行像素有多少字节）
    int dstLineSize = m_NativeWindowBuffer.stride * 4;//RGBA 缓冲区步长

    for (int i = 0; i < m_RenderHeight; ++i) {
        //一行一行地拷贝图像数据
        memcpy(dstBuffer + i * dstLineSize, m_FrameBuffer + i * srcLineSize, srcLineSize);
//        memcpy(dstBuffer + i * dstLineSize, pFrame->data[0] + i * srcLineSize, srcLineSize);

    }
//解锁当前 Window ，渲染缓冲区数据
    ANativeWindow_unlockAndPost(m_NativeWindow);

}

void VideoRender::onDecodeReady(AVFormatContext * formatContext,AVCodecContext *codecContext,jobject instance){
    duration = formatContext->duration / AV_TIME_BASE;
    LOGCATE("打印时长:%lld",duration);
    class CallA : public MsgCallback {
    public:
        void call(JNIEnv* jni,jobject obj,jmethodID methodId){
            LOGCATE("call has called child jni:%p obj:%p methodId:%p",&jni,obj,methodId);
            jni->CallVoidMethod(obj,methodId,(int)duration);
        }
    } callback;
    sendMsgWithCallback(instance, DECODE_READY_DURATION, "(I)V",(MsgCallback*)&callback);
}

void VideoRender::eachPacket(AVPacket *packet, AVCodecContext *pContext) {

}

bool VideoRender::checkFrameValid(AVFrame *pFrame) {

    while (true) {
        int64_t differ = pFrame->pts - GLUtilC::master_audio_clock;
//        LOGCATE("当前视频帧值:%jd  音频帧值%jd   当前差值differ:%jd",pFrame->pts,GLUtilC::master_audio_clock,differ);
        if (abs(differ) <= STANDRAD_VALUE) {
            break;
        }
        if (differ > 0) {
//            LOGCATE("当前超前了，需要等待");
            usleep(10 * 1000);
            continue;
        }
        if (differ < 0 && abs(differ) > 25 * 1000) {
//            LOGCATE("当前视频滞后了");
            return false;
        }
    }

    return true;
}


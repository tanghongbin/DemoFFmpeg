//
// Created by Admin on 2020/9/16.
//

#ifndef DEMOC_VIDEORENDER_H
#define DEMOC_VIDEORENDER_H

#include "BaseRender.h"
#include <android/native_window.h>
#include <android/native_window_jni.h>

extern "C" {
#include <libswscale/swscale.h>
}

#define STANDRAD_VALUE  25 * 1000


class VideoRender : public BaseRender {

private:
    SwsContext *m_SwsContext;
    int m_VideoWidth;
    int m_VideoHeight;
    ANativeWindow *m_NativeWindow;
    ANativeWindow_Buffer m_NativeWindowBuffer;

    AVFrame *m_RGBAFrame;
    uint8_t *m_FrameBuffer;
    time_t lastTime;
    long long mLastTime;
    static int64_t duration;


public:

    static int m_RenderWidth;
    static int m_RenderHeight;

    void init(AVCodecContext *codeCtx, _jobject *instance, _jobject *pJobject);

    void draw_frame(AVCodecContext *pContext, AVFrame *pFrame, _jobject *pJobject);

    void unInit();

    void displayToSurface(AVFrame *pFrame);

    bool checkFrameValid(AVFrame *pFrame);

    void eachPacket(AVPacket *packet, AVCodecContext *pContext);

    int getRenderType(){
        return 1;
    }
    void onDecodeReady(AVFormatContext * formatContext,AVCodecContext *codecContext,jobject instance);



};


#endif //DEMOC_VIDEORENDER_H

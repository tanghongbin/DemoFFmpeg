//
// Created by Admin on 2020/9/16.
//

#ifndef DEMOC_VIDEORENDER_H
#define DEMOC_VIDEORENDER_H

#define STANDRAD_VALUE  25 * 1000

#include "BaseRender.h"

extern "C" {
#include <libswscale/swscale.h>
}

class VideoRender : public BaseRender {

private:
    SwsContext *m_SwsContext;
    int m_VideoWidth;
    int m_VideoHeight;
    int m_RenderWidth;
    int m_RenderHeight;
    ANativeWindow *m_NativeWindow;
    ANativeWindow_Buffer m_NativeWindowBuffer;

    AVFrame *m_RGBAFrame;
    uint8_t *m_FrameBuffer;
    time_t lastTime;

public:

    void init(AVCodecContext *pContext, JNIEnv *pEnv, _jobject *pJobject);

    void draw_frame(AVCodecContext *pContext, AVFrame *pFrame, JNIEnv *pEnv, _jobject *pJobject);

    void unInit();

    void displayToSurface(AVFrame *pFrame);

    bool checkFrameValid(AVFrame *pFrame);

    void eachPacket(AVPacket *packet, AVCodecContext *pContext);
};


#endif //DEMOC_VIDEORENDER_H

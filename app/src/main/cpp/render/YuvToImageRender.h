//
// Created by Admin on 2020/9/27.
//

#ifndef DEMOC_YUVTOIMAGERENDER_H
#define DEMOC_YUVTOIMAGERENDER_H


#include "BaseRender.h"

extern "C" {
#include <libswscale/swscale.h>
}

class YuvToImageRender : public BaseRender  {

private:
    SwsContext *m_SwsContext;
    AVCodecContext* mCodeCtx;
    int m_VideoWidth;
    int m_VideoHeight;
    int m_RenderWidth;
    int m_RenderHeight;

    AVFrame *m_RGBAFrame;
    uint8_t *m_FrameBuffer;
    time_t lastTime;

public:

    static int mWindowWidth;
    static int mWindowHeight;

    void init(AVCodecContext *pContext,_jobject *instance, _jobject *pJobject);

    void draw_frame(AVCodecContext *pContext, AVFrame *pFrame,  _jobject *pJobject);

    void unInit();

    void eachPacket(AVPacket *packet, AVCodecContext *pContext);
};



#endif //DEMOC_YUVTOIMAGERENDER_H

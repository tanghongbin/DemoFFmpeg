//
// Created by Admin on 2020/10/12.
//

#ifndef DEMOC_OPENGLFFMPEGDECODER_H
#define DEMOC_OPENGLFFMPEGDECODER_H


#include "BaseRender.h"

class OpenGLFFmpegDecoder : public BaseRender {

private:
    SwsContext *m_SwsContext;
    AVFrame *m_RGBAFrame;
    uint8_t *m_FrameBuffer;
    int m_VideoWidth;
    int m_VideoHeight;
    int m_RenderWidth;
    int m_RenderHeight;

public:
    void init(AVCodecContext *pContext, _jobject *instance, _jobject *pJobject);

    void draw_frame(AVCodecContext *pContext, AVFrame *pFrame, _jobject *pJobject);

    void unInit();

    void eachPacket(AVPacket *packet, AVCodecContext *pContext);
};


#endif //DEMOC_OPENGLFFMPEGDECODER_H

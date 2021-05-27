//
// Created by Admin on 2020/10/12.
//

#ifndef DEMOC_OPENGLFFMPEGDECODER_H
#define DEMOC_OPENGLFFMPEGDECODER_H


#include "BaseRender.h"
#include "BaseDataCoverter.h"
#include "VideoRender.h"

extern "C" {
#include "../../include/libavcodec/avcodec.h"
#include "../../include/libavutil/frame.h"
#include <libswscale/swscale.h>
#include <libavutil/imgutils.h>
}

class OpenGLFFmpegConverter : public BaseDataCoverter {

private:
    int m_VideoWidth;
    int m_VideoHeight;
    static int m_RenderWidth;
    static int m_RenderHeight;

public:

     void Init(AVCodecContext* codecContext);
     void covertData(AVFrame *data);
     void Destroy();

};


#endif //DEMOC_OPENGLFFMPEGDECODER_H

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

class FFmpegVideoConverter : public BaseDataCoverter {

private:

public:

     void Init(AVCodecContext* codecContext);
     void covertData(DataConvertInfo *data);
     void Destroy();

};


#endif //DEMOC_OPENGLFFMPEGDECODER_H

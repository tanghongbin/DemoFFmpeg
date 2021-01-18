//
// Created by Admin on 2020/11/4.
//

#ifndef DEMOC_SWSCONVERTYUVTOPNG_H
#define DEMOC_SWSCONVERTYUVTOPNG_H



#include "SwsHelper.h"

extern "C"{
#include "../../include/libavcodec/avcodec.h"
#include "../../include/libavutil/frame.h"
};

class SwsConvertYuvToPng {
public:
    /**
        * 测试yuv原图编码成jpg，或者png，再进行缩小放大，拉伸等操作
        * @param inputFile
        * @return
        */
    static void encode(const char * inputFile);

    static void
    encodeInternal(AVCodecContext *pContext, AVFrame *pFrame, AVPacket *pPacket, FILE *pFile,
                   SwsHelper *pHelper);
};


#endif //DEMOC_SWSCONVERTYUVTOPNG_H

//
// Created by Admin on 2020/10/16.
//

#ifndef DEMOC_ENCODEYUVTOJPG_H
#define DEMOC_ENCODEYUVTOJPG_H


#include "SwsHelper.h"

extern "C"{
#include <libavcodec/avcodec.h>
};


#define INPUT_YUV_IMAGE_PATH "/storage/emulated/0/ffmpegtest/YUV_Image_840x1074.NV21"
#define OUTPUT_JPG_IMAGE_PATH "/storage/emulated/0/ffmpegtest/test_out_file.jpg"
#define OUTPUT_PNG_IMAGE_PATH "/storage/emulated/0/ffmpegtest/test_png_out_file.png"

#define OUTPUT_SWS_PNG_IMAGE_PATH "/storage/emulated/0/ffmpegtest/sws_after_scale.png"

class EncodeYuvToJpg {

public:

    /**
     * 测试yuv原图编码成jpg，或者png，再进行缩小放大，拉伸等操作
     * @param inputFile
     * @return
     */
    static const char * encode(const char * inputFile);

    static void
    encodeInternal(AVCodecContext *pContext, AVFrame *pFrame, AVPacket *pPacket, FILE *pFile,
                   SwsHelper *pHelper);

};


#endif //DEMOC_ENCODEYUVTOJPG_H

//
// Created by Admin on 2020/10/16.
//

#ifndef DEMOC_ENCODEYUVTOJPG_H
#define DEMOC_ENCODEYUVTOJPG_H


extern "C"{
#include <libavcodec/avcodec.h>
};

class EncodeYuvToJpg {

public:
    static const char * encode(const char * inputFile);

    static void encodeInternal(AVCodecContext *pContext, AVFrame *pFrame, AVPacket *pPacket, FILE *pFile);
};


#endif //DEMOC_ENCODEYUVTOJPG_H

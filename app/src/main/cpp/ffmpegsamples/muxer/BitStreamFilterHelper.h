//
// Created by Admin on 2020/11/19.
//

#ifndef DEMOC_BITSTREAMFILTERHELPER_H
#define DEMOC_BITSTREAMFILTERHELPER_H

extern "C"{
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
};


class BitStreamFilterHelper {

private:
    AVBitStreamFilterContext* h264bsfc;
    AVBitStreamFilterContext* aacbsfc;


public:

    void initAudio();

    void initVideo();

    void filterAudio(AVStream *inStream, AVPacket *pkt);

    void filterVideo(AVStream *inStream, AVPacket *pkt);

    void freeCtx();

};


#endif //DEMOC_BITSTREAMFILTERHELPER_H

//
// Created by Admin on 2020/11/19.
//


#include "BitStreamFilterHelper.h"


void BitStreamFilterHelper::initAudio() {
    aacbsfc = av_bitstream_filter_init("aac_adtstoasc");
}

void BitStreamFilterHelper::initVideo() {
    h264bsfc = av_bitstream_filter_init("h264_mp4toannexb");
}

void BitStreamFilterHelper::filterAudio(AVStream *inStream, AVPacket *pkt) {
    av_bitstream_filter_filter(aacbsfc, inStream->codec, NULL, &pkt->data, &pkt->size, pkt->data,
                               pkt->size, 0);
}

void BitStreamFilterHelper::filterVideo(AVStream *inStream, AVPacket *pkt) {
    av_bitstream_filter_filter(h264bsfc, inStream->codec, NULL, &pkt->data, &pkt->size, pkt->data,
                               pkt->size, 0);
}

void BitStreamFilterHelper::freeCtx(){
    if (aacbsfc){
        av_bitstream_filter_close(aacbsfc);
    }
    if (h264bsfc){
        av_bitstream_filter_close(h264bsfc);
    }
}

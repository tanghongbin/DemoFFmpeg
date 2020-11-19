//
// Created by Admin on 2020/11/19.
//

#ifndef DEMOC_CUSTOMMUXER_H
#define DEMOC_CUSTOMMUXER_H


extern "C"{
#include <libavformat/avformat.h>
};

class CustomMuxer {

public:
    /**
     * 混合音视频流进行输出
     */
    static void muxer();

    /**
     * 添加流到fotmatctx中
     * @param pContext
     * @param pContext1
     * @param index
     * @return
     */
    static int
    addNewStream(AVFormatContext *pContext, AVFormatContext *pContext1, int index, int *pInt);
};


#endif //DEMOC_CUSTOMMUXER_H

//
// Created by Admin on 2021/11/22.
//

#ifndef DEMOFFMPEG_IMLHWDECODER_H
#define DEMOFFMPEG_IMLHWDECODER_H

#include "BaseHwDecoder.h"

class HwAudioDecoder : public BaseHwDecoder {
    char * getDecodeTypeStr() {
        return "audio";
    }
    void setMediaType(int mediaType) {
        this->appointMediaType = mediaType;
        audioRender = new OpenSLESRender;
        audioRender->Init();
        LOGCATE("音频渲染初始化完毕");
    }

};

class HwVideoDecoder : public BaseHwDecoder {
    char * getDecodeTypeStr() {
        return "video";
    }
};

#endif //DEMOFFMPEG_IMLHWDECODER_H

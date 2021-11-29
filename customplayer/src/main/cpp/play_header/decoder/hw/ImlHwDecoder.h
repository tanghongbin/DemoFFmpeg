//
// Created by Admin on 2021/11/22.
//

#ifndef DEMOFFMPEG_IMLHWDECODER_H
#define DEMOFFMPEG_IMLHWDECODER_H

#include "BaseHwDecoder.h"

class HwAudioDecoder : public BaseHwDecoder {
private:
    HwAudioDecoder() {};
    static HwAudioDecoder* instance;
public:
    char * getDecodeTypeStr() {
        return "audio";
    }
    void setMediaType(int mediaType) {
        this->appointMediaType = mediaType;
        audioRender = new OpenSLESRender;
        audioRender->Init();
        LOGCATE("音频渲染初始化完毕");
    }
    static HwAudioDecoder* getInstance(){
        if (instance == nullptr) {
            instance = new HwAudioDecoder;
        }
        return instance;
    }

    static void destroyInstance(){
        if (instance) {
            delete instance;
            instance = 0;
        }
    }

};

class HwVideoDecoder : public BaseHwDecoder {
    char * getDecodeTypeStr() {
        return "video";
    }
    int64_t getCurrentAudioPtsUs(){
        std::lock_guard<std::mutex> ptsMutex(audioPtsMutex);
        return HwAudioDecoder::getInstance()->getCurrentAudioPtsUs();
    }
};

#endif //DEMOFFMPEG_IMLHWDECODER_H

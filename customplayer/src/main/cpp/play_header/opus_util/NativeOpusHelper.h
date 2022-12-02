//
// Created by Admin on 2022/11/22.
//

#ifndef DEMOFFMPEG_NATIVEOPUSHELPER_H
#define DEMOFFMPEG_NATIVEOPUSHELPER_H

#include <cstdio>
#include <opus/opus.h>
#include "../../play_header/utils/AudioRecordPlayHelper.h"

class NativeOpusHelper{
private:
    AudioRecordPlayHelper* audioRecordPlayHelper;
    static void staticReceiveProduceAudio(uint8_t* data,int size);
    void resolvePcmAudio(uint8_t* data,int size);
    static NativeOpusHelper* instance;
    static NativeOpusHelper* getInstance();
    static void realStartCapture();
    static void realStartDecode();
    FILE* file;
    FILE* tempPcmFile;
    OpusEncoder* opusEncoder;
    std::thread* encodeThread;
    std::thread* decodeThread;
    int64_t startTime;
    const char * opusPath;
    const char * tempPcmPath;
public:
    NativeOpusHelper();
    ~NativeOpusHelper();
    void init();
    void startEncode();
    void stopEncode();
    void startPlay();
    void playPcm();
    void destroy();
};

#endif //DEMOFFMPEG_NATIVEOPUSHELPER_H

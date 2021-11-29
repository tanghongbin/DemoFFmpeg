//
// Created by Admin on 2021/5/24.
//

#ifndef DEMOFFMPEG_IMLFFMPEGDECODER_H
#define DEMOFFMPEG_IMLFFMPEGDECODER_H

#include "BaseFFmpegDecoder.h"
#include <cstdint>
#include <render/VideoRender.h>

class AudioFFmpegDecoder : public BaseFFmpegDecoder{
private:
    AudioFFmpegDecoder(){}
    static AudioFFmpegDecoder* instance;
public:
    BaseDataCoverter * createConverter();
    int64_t getCurrentAudioPtsUs() {
        return currentAudioPts;
    }
    static AudioFFmpegDecoder* getInstance();
    static void destroyInstance();
};

class VideoFFmpegDecoder : public BaseFFmpegDecoder{
protected:
    BaseDataCoverter * createConverter();
    int windowWidth;
    int windowHeight;
    // 1-竖屏，2-横屏
    int mOreration;
public:
    VideoFFmpegDecoder();
    void drawVideoFrame();
    void OnSurfaceChanged(int oreration,int width,int height);
    void OnSizeReady();
    int64_t getCurrentAudioPtsUs() {
        return AudioFFmpegDecoder::getInstance()->getCurrentAudioPtsUs();
    }
};

#endif //DEMOFFMPEG_IMLFFMPEGDECODER_H

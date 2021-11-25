//
// Created by Admin on 2021/11/19.
//

#ifndef DEMOFFMPEG_BASEDECODER_H
#define DEMOFFMPEG_BASEDECODER_H

#include "../render/BaseDataCoverter.h"
#include "../media/OpenSLESRender.h"
#include "../utils/utils.h"
#include <mutex>
#include <thread>

class BaseDecoder {
protected:
    // 1-音频，2-视频
    int appointMediaType;
    virtual BaseDataCoverter * createConverter() = 0;
    virtual int64_t getCurrentAudioPts() {};
public:
    VideoRender* videoRender;
    OpenSLESRender* audioRender;
    BaseDecoder() {}
    virtual void setVideoRender(VideoRender* render) {
        this->videoRender = render;
    }
    virtual void setMediaType(int mediaType) {
        this->appointMediaType = mediaType;
    };
    virtual void Init(const char * url) = 0 ;
    virtual void Destroy() = 0;
    virtual void Reset() = 0;
    virtual void Start() = 0;
    virtual void Stop() = 0;
    virtual void ManualSeekPosition(int position) = 0;
    virtual void Replay() = 0;
};


#endif //DEMOFFMPEG_BASEDECODER_H

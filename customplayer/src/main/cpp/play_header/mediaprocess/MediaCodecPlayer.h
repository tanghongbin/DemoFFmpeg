//
// Created by Admin on 2021/5/13.
//
// 硬解码播放器
#ifndef DEMOFFMPEG_MEDIACODECMEDIAPLAYER_H
#define DEMOFFMPEG_MEDIACODECMEDIAPLAYER_H

#include <decoder/BaseDecoder.h>
#include <render/BaseRender.h>
#include "AbsCustomMediaPlayer.h"

class MediaCodecPlayer : public AbsCustomMediaPlayer{
private:
    BaseDecoder* audioDecoder;
    BaseDecoder* videoDecoder;
    BaseRender* audioRender;
    BaseRender* videoRender;

public:
    void Init();
    void OnSurfaceCreated();
    void OnSurfaceChanged(int width,int height);
    void OnDrawFrame();
    void Destroy();
};

#endif //DEMOFFMPEG_MEDIACODECMEDIAPLAYER_H

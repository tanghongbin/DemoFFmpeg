//
// Created by Admin on 2021/5/13.
//
// 硬解码播放器
#ifndef DEMOFFMPEG_MEDIACODECMEDIAPLAYER_H
#define DEMOFFMPEG_MEDIACODECMEDIAPLAYER_H

#include <decoder/BaseFFmpegDecoder.h>
#include <render/BaseRender.h>
#include <decoder/hw/BaseHwDecoder.h>
#include "AbsCustomMediaPlayer.h"

class MediaCodecPlayer : public AbsCustomMediaPlayer{
private:
    BaseHwDecoder* audioDecoder;
    BaseHwDecoder* videoDecoder;

public:
    MediaCodecPlayer();
    void Init();
    void OnSurfaceCreated();
    void OnSurfaceChanged(int oreration,int width,int height);
    void OnDrawFrame();
    void Destroy();
    void Prepare() ;
    void Start() ;
    void Stop() ;
    void Reset();
    void SeekTo(int second) ;
    void Replay();
};

#endif //DEMOFFMPEG_MEDIACODECMEDIAPLAYER_H

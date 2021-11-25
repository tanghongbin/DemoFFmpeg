//
// Created by Admin on 2021/5/13.
//

#ifndef DEMOFFMPEG_FFMPEGMEDIAPLAYER_H
#define DEMOFFMPEG_FFMPEGMEDIAPLAYER_H

#include <decoder/BaseFFmpegDecoder.h>
#include "AbsCustomMediaPlayer.h"

class FFmpegMediaPlayer : public AbsCustomMediaPlayer{

private:
    BaseFFmpegDecoder* audioDecoder = 0;
    BaseFFmpegDecoder* videoDecoder = 0;

public:
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

#endif //DEMOFFMPEG_FFMPEGMEDIAPLAYER_H

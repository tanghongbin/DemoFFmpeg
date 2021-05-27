//
// Created by Admin on 2021/5/13.
//

#ifndef DEMOFFMPEG_FFMPEGMEDIAPLAYER_H
#define DEMOFFMPEG_FFMPEGMEDIAPLAYER_H

#include <decoder/BaseDecoder.h>
#include "AbsCustomMediaPlayer.h"

class FFmpegMediaPlayer : public AbsCustomMediaPlayer{

private:
//    BaseDecoder* audioDecoder = 0;
    BaseDecoder* videoDecoder = 0;

public:
    void Init();
    void OnSurfaceCreated();
    void OnSurfaceChanged(int width,int height);
    void OnDrawFrame();
    void Destroy();
    void Prepare() ;
    void Start() ;
    void Stop() ;
    void SeekTo(int second) ;
};

#endif //DEMOFFMPEG_FFMPEGMEDIAPLAYER_H

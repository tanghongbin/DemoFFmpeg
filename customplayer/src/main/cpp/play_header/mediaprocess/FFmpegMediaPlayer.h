//
// Created by Admin on 2021/5/13.
//

#ifndef DEMOFFMPEG_FFMPEGMEDIAPLAYER_H
#define DEMOFFMPEG_FFMPEGMEDIAPLAYER_H

#include <decoder/BaseDecoder.h>
#include "AbsCustomMediaPlayer.h"

class FFmpegMediaPlayer : public AbsCustomMediaPlayer{

private:
    BaseDecoder* audioDecoder;
    BaseDecoder* videoDecoder;

public:
    void Init();
    void OnSurfaceCreated();
    void OnSurfaceChanged(int width,int height);
    void OnDrawFrame();
    void Destroy();
    void Prepare() ;
    void Start() ;
    void Stop() ;
    void SeekTo(long second) ;
};

#endif //DEMOFFMPEG_FFMPEGMEDIAPLAYER_H

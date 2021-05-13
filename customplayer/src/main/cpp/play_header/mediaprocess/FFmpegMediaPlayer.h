//
// Created by Admin on 2021/5/13.
//

#ifndef DEMOFFMPEG_FFMPEGMEDIAPLAYER_H
#define DEMOFFMPEG_FFMPEGMEDIAPLAYER_H

#include "AbsCustomMediaPlayer.h"

class FFmpegMediaPlayer : public AbsCustomMediaPlayer{
    void Init();
    void OnSurfaceCreated();
    void OnSurfaceChanged(int width,int height);
    void OnDrawFrame();
    void Destroy();
};

#endif //DEMOFFMPEG_FFMPEGMEDIAPLAYER_H

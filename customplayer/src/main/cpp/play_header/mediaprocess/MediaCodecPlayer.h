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
#include <android/native_window_jni.h>

class MediaCodecPlayer : public AbsCustomMediaPlayer{
private:
    BaseHwDecoder* audioDecoder;
    BaseHwDecoder* videoDecoder;
    ANativeWindow* mNativeWindow;

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
    void setNativeWindow(jobject surface);
    void deleteNativeWindow();
};

#endif //DEMOFFMPEG_MEDIACODECMEDIAPLAYER_H

//
// Created by Admin on 2021/5/13.
//
// 硬解码播放器
#ifndef DEMOFFMPEG_MEDIACODECMEDIAPLAYER_H
#define DEMOFFMPEG_MEDIACODECMEDIAPLAYER_H

#include <decoder/BaseFFmpegDecoder.h>
#include <render/BaseRender.h>
#include <decoder/hw/BaseHwDecoder.h>
#include <utils/Callback.h>
#include <encoder/SpecialEffortsVideoMuxer.h>
#include "AbsCustomMediaPlayer.h"

class MediaCodecPlayer : public AbsCustomMediaPlayer, Callback,OnReadPixelListener {
private:
    BaseHwDecoder* audioDecoder;
    BaseHwDecoder* videoDecoder;
    BaseHwDecoder* specialEffortDecoder; // 特效视频
    SpecialEffortsVideoMuxer* specialEffortsVideoMuxer;
    MediaCodecPlayer();
    static MediaCodecPlayer* instance;
    static void receiveSpecialEffortsData(int type,AMediaCodecBufferInfo* info,uint8_t* data);
public:
    static MediaCodecPlayer* getInstance();
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
    void ApplyEfforts(const char * url);
    void call();
    void readPixelResult(NativeOpenGLImage* data);
};

#endif //DEMOFFMPEG_MEDIACODECMEDIAPLAYER_H

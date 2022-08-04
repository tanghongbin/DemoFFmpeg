//
// Created by Admin on 2021/5/31.
//

#ifndef DEMOFFMPEG_RTMPLIVEMUXER_H
#define DEMOFFMPEG_RTMPLIVEMUXER_H

#include "../utils/CustomSafeBlockQueue.h"
#include "../render/BaseRender.h"
#include "AbsMediaMuxer.h"
#include "../utils/OpenGLImageDef.h"
#include "EncoderAACAndx264.h"
#include <thread>
#include "utils/Constants.h"
/***
 * 直播x264 编码 推流
 */
class RtmpLiveMuxer : public AbsMediaMuxer{
private:
    static RtmpLiveMuxer* instance;
    int audioTrackIndex,videoTrackIndex;
    EncoderAACAndx264* mAvEncoder;
    bool isStartEncode;
    static void receivePixelData(int type,NativeOpenGLImage *pVoid);
    static void receiveAudioData(short *audioData, int length);
    RtmpLiveMuxer();
    bool isInit;
public:
    bool isDestroyed;
    std::mutex runningMutex;
    int init(const char * outFileName);
    void Start();
    void Stop();
    void Destroy();
     static RtmpLiveMuxer* getInstance(){
        if (instance == nullptr){
            instance = new RtmpLiveMuxer;
        }
        return instance;
    }
    void getInAvDataFunc(ReceiveAvOriginalData* data);
    void configAudioPrams(int samHz,int chnns);
};

#endif //DEMOFFMPEG_RTMPLIVEMUXER_H

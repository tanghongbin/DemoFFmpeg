//
// Created by Admin on 2021/5/31.
//

#ifndef DEMOFFMPEG_SPECIALEFFORTSMUXER_H
#define DEMOFFMPEG_SPECIALEFFORTSMUXER_H


#include <thread>
#include "../utils/CustomSafeBlockQueue.h"
#include "../render/BaseRender.h"
#include "hw/NdkMediaMuxerHelper.h"
#include "AbsMediaMuxer.h"
#include "hw/MediaCodecVideo.h"

/***
 * 特效视频 录制
 */
class SpecialEffortsVideoMuxer : public AbsMediaMuxer{
private:
    static SpecialEffortsVideoMuxer* instance;
    char* mTargetPathV;
    NdkMediaMuxerHelper* mMuxerHelper;
    MediaCodecVideo* mediaCodecV;
    int videoTrackIndex;
    bool isStarted;
    static void receiveMediaCodecData(int type,AMediaCodecBufferInfo * bufferInfo, uint8_t* data);
    static void receiveCodecFmtChanged(int type,AMediaFormat* mediaFormat);
    SpecialEffortsVideoMuxer(){
        instance = 0;
        videoTrackIndex = 0;
        isDestroyed = false;
        isStarted = false;
        mediaCodecV = 0;
        mTargetPathV = 0;
    }
public:
    static SpecialEffortsVideoMuxer* getInstance(){
        if (instance == nullptr){
            instance = new SpecialEffortsVideoMuxer;
        }
        return instance;
    }
    static void receiveOriginalAvData(int type,void * pVoid,int size);

    bool isDestroyed;
    std::mutex runningMutex;
    int init(const char * outFileName);
    void Stop();
    void Destroy();
    void setVideoConfigInfo(VideoConfigInfo * configInfo);
};

#endif //DEMOFFMPEG_SPECIALEFFORTSMUXER_H

//
// Created by Admin on 2021/5/31.
//

#ifndef DEMOFFMPEG_SVMUXER_H
#define DEMOFFMPEG_SVMUXER_H


#include <thread>
#include "../utils/CustomSafeBlockQueue.h"
#include "../render/BaseRender.h"
#include "hw/NdkMediaMuxerHelper.h"
#include "AbsMediaMuxer.h"
#include "hw/MediaCodecAudio.h"
#include "hw/MediaCodecVideo.h"
#include <utils/SoundTouchHelper.h>

/***
 * 短视频录制
 */
class ShortVideoMuxer : public AbsMediaMuxer{
private:

    static ShortVideoMuxer* instance;
    std::thread * thread;
    char mTargetFilePath[128];
    FILE* mTargetFileA;
    char* mTargetPathA;
    char* mTargetPathV;
    NdkMediaMuxerHelper* mMuxerHelper;
    MediaCodecAudio* mediaCodecA;
    MediaCodecVideo* mediaCodecV;
    int audioTrackIndex,videoTrackIndex;
    bool isStarted;
    static void receiveMediaCodecData(int type,AMediaCodecBufferInfo * bufferInfo, uint8_t* data);
    static void receiveCodecFmtChanged(int type,AMediaFormat* mediaFormat);
    static void receiveOriginalAvData(int type,void * pVoid,int size);
    ShortVideoMuxer(){
        audioTrackIndex = videoTrackIndex = 0;
        thread = 0;
        isDestroyed = false;
        isStarted = false;
        mediaCodecA = 0;
        mediaCodecV = 0;
        mTargetPathA = mTargetPathV = 0;
        mTargetFileA = 0;
    }
public:
    bool isDestroyed;
    std::mutex runningMutex;
    int init(const char * outFileName);
    void Stop();
    void Destroy();
    static ShortVideoMuxer* getInstance(){
        if (instance == nullptr){
            instance = new ShortVideoMuxer;
        }
        return instance;
    }

    void addAtdsHeader(uint8_t* data, int32_t size);
};

#endif //DEMOFFMPEG_HWMEDIAMUXER_H

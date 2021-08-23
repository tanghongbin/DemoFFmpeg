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

/***
 * 直播x264 编码 推流
 */
class RtmpLiveMuxer : public AbsMediaMuxer{
private:

    static RtmpLiveMuxer* instance;
    std::thread * thread;
    char mTargetFilePath[128];
    BaseVideoRender* videoRender;
    int audioTrackIndex,videoTrackIndex;
    EncoderAACAndx264* mAvEncoder;
    static void receivePixelData(int type,NativeOpenGLImage *pVoid);
    RtmpLiveMuxer(){
        audioTrackIndex = videoTrackIndex = 0;
        thread = 0;
        videoRender = 0;
        isDestroyed = false;
        cameraWidth = cameraHeight = 0;
        mAvEncoder = 0;
    }
public:
    bool isDestroyed;
    std::mutex runningMutex;
    int cameraWidth,cameraHeight;
    int init(const char * outFileName);
    void Destroy();
     static RtmpLiveMuxer* getInstance(){
        if (instance == nullptr){
            instance = new RtmpLiveMuxer;
        }
        return instance;
    }

    void OnSurfaceCreate();
    void OnSurfaceChanged(int width,int height);
    void OnCameraFrameDataValible(int type,NativeOpenGLImage* data);
    void OnDrawFrame();
    void OnAudioData(uint8_t *audioData, int length);
};

#endif //DEMOFFMPEG_RTMPLIVEMUXER_H

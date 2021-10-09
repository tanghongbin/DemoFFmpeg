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
    BaseVideoRender* videoRender;
    int audioTrackIndex,videoTrackIndex;
    EncoderAACAndx264* mAvEncoder;
    bool isStartEncode;
    static void receivePixelData(int type,NativeOpenGLImage *pVoid);
    RtmpLiveMuxer();
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
    void configAudioPrams(int samHz,int chnns);

    /**
     * 转换格式  转换成 i420
     * @param srcData
     */
    void formatConvert(const NativeOpenGLImage *srcData);
};

#endif //DEMOFFMPEG_RTMPLIVEMUXER_H

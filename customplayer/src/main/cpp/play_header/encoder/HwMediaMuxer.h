//
// Created by Admin on 2021/5/31.
//

#ifndef DEMOFFMPEG_HWMEDIAMUXER_H
#define DEMOFFMPEG_HWMEDIAMUXER_H


#include <thread>
#include "../utils/CustomSafeBlockQueue.h"
#include "../render/BaseRender.h"
#include "hw/NdkMediaMuxerHelper.h"
#include "AbsMediaMuxer.h"
#include "hw/MediaCodecAudio.h"
#include "hw/MediaCodecVideo.h"
#include <utils/SoundTouchHelper.h>

/***
 * 硬编码录制
 */
class HwMediaMuxer : public AbsMediaMuxer{
private:

    static HwMediaMuxer* instance;
    std::thread * thread;
    char mTargetFilePath[128];
    BaseVideoRender* videoRender;
    NdkMediaMuxerHelper* mMuxerHelper;
    MediaCodecAudio* mediaCodecA;
    MediaCodecVideo* mediaCodecV;
    int audioTrackIndex,videoTrackIndex;
    SoundTouchHelper* soundTouchHelper;
    bool isStarted;
    static void receiveMediaCodecData(int type,AMediaCodecBufferInfo * bufferInfo, uint8_t* data);
    static void receiveCodecFmtChanged(int type,AMediaFormat* mediaFormat);
    static void receivePixelData(int type,NativeOpenGLImage *pVoid);
    /***
     * 接受soundtouch 转换之后的数据
     * @param data
     * @param size
     */
    static void receiveSoundTouchData(short * data,int size);
    HwMediaMuxer(){
        soundTouchHelper = 0;
        audioTrackIndex = videoTrackIndex = 0;
        thread = 0;
        videoRender = 0;
        isDestroyed = false;
        isStarted = false;
        mediaCodecA = 0;
        mediaCodecV = 0;
        cameraWidth = cameraHeight = 0;
    }
public:
    bool isDestroyed;
    std::mutex runningMutex;
    int cameraWidth,cameraHeight;
    int init(const char * outFileName);
    void Destroy();
    static HwMediaMuxer* getInstace(){
        if (instance == nullptr){
            instance = new HwMediaMuxer;
        }
        return instance;
    }

    void OnSurfaceCreate();
    void OnSurfaceChanged(int width,int height);
    void OnCameraFrameDataValible(int type,NativeOpenGLImage* data);
    void OnDrawFrame();
    void OnAudioData(uint8_t *audioData, int length);
};

#endif //DEMOFFMPEG_HWMEDIAMUXER_H

//
// Created by Admin on 2021/11/9.
//

#ifndef DEMOFFMPEG_OUTPUTDISPLAYHELPER_H
#define DEMOFFMPEG_OUTPUTDISPLAYHELPER_H

#include "../utils/utils.h"
#include <cstdint>
#include <render/BaseRender.h>
#include <utils/SoundTouchHelper.h>
#include <encoder/OutputDisplayHelper.h>
#include <render/VideoFboRender.h>
#include <render/VideoFboOESRender.h>

/***
 * //级慢
    VERY_SLOW(0.25),
    //慢
    SLOW(0.5),
    //正常
    NORMAL(1.0),
    //快
    FAST(2.0),
    //极快
    VERY_FAST(3.0)
 */


/***
 * 视频录制帮助类，通过回调输出改频后的音频和oes纹理数据
 * in  视频-oes纹理处理，音频-pcm数据 OnAudioData
 *
 * out， receivePixelData 接受videorender渲染后的数据，  receiveSoundTouchData 接受soundtouch
 * 处理后的数据
 *
 * 约定 最后处理数据的一方进行拷贝，中间不做处理
 *
 */
class OutputDisplayHelper {
private:
    ReceiveAudioData audioCall;
    ReceiveOqTypeData videoCall;
    BaseVideoRender* videoRender;
    static OutputDisplayHelper* instance;
    bool isDestroyed;
    std::mutex runningMutex;
    float speed;
    SoundTouchHelper* soundTouchHelper;


    static void receivePixelData(int type,NativeOpenGLImage *pVoid);
    /***
     * 接受soundtouch 转换之后的数据
     * @param data
     * @param size
     */
    static void receiveSoundTouchData(short * data,int size);
    OutputDisplayHelper(){
        videoRender = 0;
        isDestroyed = false;
        audioCall = nullptr;
        videoCall = 0;
        soundTouchHelper = 0;
        speed = 1.0f;
    }

public:
    ~OutputDisplayHelper();
    static OutputDisplayHelper * getInstance();
    void init();
    void setSpeed(float speed);
    void setOutputAudioListener(ReceiveAudioData outputAvData);
    void setOutputVideoListener(ReceiveOqTypeData outputAvData);
    void OnSurfaceCreate();
    void OnSurfaceChanged(int width,int height);
    void OnCameraFrameDataValible(int type,NativeOpenGLImage* data);
    void OnDrawFrame();
    void OnAudioData(uint8_t *audioData, int length);

    void UpdateOESMartix(float *pDouble);
};

#endif //DEMOFFMPEG_OUTPUTDISPLAYHELPER_H

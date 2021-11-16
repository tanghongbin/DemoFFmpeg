//
// Created by Admin on 2021/11/9.
//

#ifndef DEMOFFMPEG_OUTPUTDISPLAYHELPER_H
#define DEMOFFMPEG_OUTPUTDISPLAYHELPER_H

#include "../utils/utils.h"
#include <cstdint>
#include <render/BaseRender.h>
#include <utils/SoundTouchHelper.h>

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
 * 输出，包括原生音频和渲染后从GPU 读取的视频-rgba,
 * 这里做一个开关
 */
class OutputDisplayHelper {
private:
    OutputAvData outputAvData;
    BaseVideoRender* videoRender;
    static OutputDisplayHelper* instance;
    bool isDestroyed;
    int cameraWidth,cameraHeight;
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
        outputAvData = nullptr;
        cameraWidth = cameraHeight = 0;
        soundTouchHelper = 0;
        speed = 1.0f;
    }

public:
    static OutputDisplayHelper * getInstance();
    void init();
    void destroy();
    void setSpeed(float speed);
    void setOutputListener(OutputAvData outputAvData);
    void OnSurfaceCreate();
    void OnSurfaceChanged(int width,int height);
    void OnCameraFrameDataValible(int type,NativeOpenGLImage* data);
    void OnDrawFrame();
    void OnAudioData(uint8_t *audioData, int length);
};

#endif //DEMOFFMPEG_OUTPUTDISPLAYHELPER_H

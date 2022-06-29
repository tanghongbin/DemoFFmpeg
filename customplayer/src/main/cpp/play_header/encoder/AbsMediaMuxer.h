//
// Created by Admin on 2021/5/31.
//

#ifndef DEMOFFMPEG_ABSMEDIAMUXER_H
#define DEMOFFMPEG_ABSMEDIAMUXER_H

#include <utils/OpenGLImageDef.h>
#include "utils/utils.h"

/***
 * 接受原生数据的回调，来源是OutputDisplayHelper
 * 音频-soundtouch改变之后的pcm数据
 * 视频-videorender 渲染之后的yuv420数据
 */
typedef struct _receive_av_data {
    ReceiveAudioData audioCall;
    ReceiveOqTypeData videoCall;
} ReceiveAvOriginalData;

class AbsMediaMuxer{
private:
    int sampleHz;
    int channels;
protected:
    double speed;
    /***
     * 临时占位的监听
     */
    ReceiveAvOriginalData* callback;
public:
    AbsMediaMuxer(){
        callback = 0;
    }
    virtual int init(const char * outFileName) = 0;
    virtual void Destroy() = 0;
    virtual void test(int type){

    }
    virtual void Stop() {};
    virtual void OnDrawFrame() {  };
    virtual void OnSurfaceCreate() {  };
    virtual void OnSurfaceChanged(int width,int height) {  };
    virtual void OnCameraFrameDataValible(int type,NativeOpenGLImage * data) {  };

    virtual void getInAvDataFunc(ReceiveAvOriginalData* data) {}

    virtual void SetPeed(double sp) {
        this->speed = sp;
    }
    virtual void OnAudioData(uint8_t *audioData, int length) {  };
    virtual void configAudioPrams(int samHz,int chnns){
        this->sampleHz = samHz;
        this->channels = chnns;
    }
};

#endif //DEMOFFMPEG_ABSMEDIAMUXER_H

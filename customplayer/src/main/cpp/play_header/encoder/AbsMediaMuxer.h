//
// Created by Admin on 2021/5/31.
//

#ifndef DEMOFFMPEG_ABSMEDIAMUXER_H
#define DEMOFFMPEG_ABSMEDIAMUXER_H

#include <utils/OpenGLImageDef.h>



class AbsMediaMuxer{
private:
    int sampleHz;
    int channels;
protected:
    double speed;
public:
    virtual int init(const char * outFileName) = 0;
    virtual void Destroy() = 0;
    virtual void test(int type){

    }
    virtual void Stop() {};
    virtual void OnDrawFrame() {  };
    virtual void OnSurfaceCreate() {  };
    virtual void OnSurfaceChanged(int width,int height) {  };
    virtual void OnCameraFrameDataValible(int type,NativeOpenGLImage * data) {  };

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

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
public:
    virtual int init(const char * outFileName) = 0;
    virtual void Destroy() = 0;
    virtual void test(int type){

    }
    virtual void OnDrawFrame() = 0;
    virtual void OnSurfaceCreate() = 0;
    virtual void OnSurfaceChanged(int width,int height) = 0;
    virtual void OnCameraFrameDataValible(int type,NativeOpenGLImage * data) = 0;

    virtual void OnAudioData(uint8_t *audioData, int length) = 0;
    virtual void configAudioPrams(int samHz,int chnns){
        this->sampleHz = samHz;
        this->channels = chnns;
    }
};

#endif //DEMOFFMPEG_ABSMEDIAMUXER_H

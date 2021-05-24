//
// Created by Admin on 2021/5/13.
//

#ifndef DEMOFFMPEG_ABSCUSTOMMEDIAPLAYER_H
#define DEMOFFMPEG_ABSCUSTOMMEDIAPLAYER_H

#include <cstdint>
#include <utils/CustomSafeQueue.h>
#include <utils/Message.h>
#include <utils/JavaVmManager.h>
#include <thread>

class AbsCustomMediaPlayer{

public:
    virtual void Init() = 0;
    virtual void OnSurfaceCreated() {  };
    virtual void OnSurfaceChanged(int width,int height) {  };
    virtual void OnDrawFrame() {  };
    virtual void DecodeFrame(uint8_t* data) {};
    virtual void Destroy() = 0;
};

#endif //DEMOFFMPEG_ABSCUSTOMMEDIAPLAYER_H

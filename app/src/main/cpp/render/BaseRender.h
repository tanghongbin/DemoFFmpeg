//
// Created by Admin on 2020/9/16.
//

#ifndef DEMOC_BASERENDER_H
#define DEMOC_BASERENDER_H

extern "C" {
#include "../include/libavcodec/avcodec.h"
#include "../include/libavutil/frame.h"
};

#include "../../../../../../../android_sdk/android_sdk/sdk/ndk-bundle/toolchains/llvm/prebuilt/windows-x86_64/sysroot/usr/include/jni.h"

class BaseRender {
public:
    virtual void init(AVCodecContext *pContext, JNIEnv *pEnv, _jobject *pJobject) = 0;

    virtual void draw_frame(AVCodecContext *pContext, AVFrame *pFrame, JNIEnv *pEnv, _jobject *pJobject) = 0;

    virtual void unInit() = 0;

    virtual void eachPacket(AVPacket *packet, AVCodecContext *pContext) = 0;

};

#endif //DEMOC_BASERENDER_H

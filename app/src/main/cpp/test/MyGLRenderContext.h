//
// Created by Admin on 2020/8/25.
//



#ifndef DEMOC_MYGLRENDERCONTEXT_H
#define DEMOC_MYGLRENDERCONTEXT_H

#include <TextureSample.h>
#include <GLYuvSample.h>
#include <GLVBOEBOSample.h>
#include "utils.h"
#include "../utils/GLUtils.h"
#include "TriangleSample.h"
#include <GLFBOSample.h>
#include <GLEglSample.h>
#include <CoordinateSystemSample.h>
#include <ParticlesSample.h>
#include "../../../../../../../android_sdk/android_sdk/sdk/ndk-bundle/toolchains/llvm/prebuilt/windows-x86_64/sysroot/usr/include/c++/v1/cstdint"


class MyGLRenderContext {

private:

    MyGLRenderContext();

    ~MyGLRenderContext();

    TextureSample *m_Sample;


    static MyGLRenderContext *m_pContext;

public:

    int width, height;

    void SetImageData(int format, int width, int height, uint8_t *pData);

    void OnSurfaceCreated();

    void OnSurfaceChanged(int width, int height);

    void OnDrawFrame();

    static MyGLRenderContext *GetInstance();

    static void DestroyInstance();

};

#endif //DEMOC_MYGLRENDERCONTEXT_H

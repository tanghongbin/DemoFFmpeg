//
// Created by Admin on 2020/8/25.
//



#ifndef DEMOC_MYGLRENDERCONTEXT_H
#define DEMOC_MYGLRENDERCONTEXT_H

#include <TextureSample.h>
#include <GLYuvSample.h>
#include <GLVBOEBOSample.h>
#include "utils.h"
#include "CustomGLUtils.h"
#include "TriangleSample.h"
#include <GLFBOSample.h>
#include <GLEglSample.h>
#include <CoordinateSystemSample.h>
#include <ParticlesSample.h>
#include <mutex>


class MyGLRenderContext {

private:

    MyGLRenderContext();

    ~MyGLRenderContext();

    GLBaseSample *m_Sample;

    std::mutex my_mutex;


    static MyGLRenderContext *m_pContext;

public:

    int width, height;

    void SetImageData(int format, int width, int height, uint8_t *pData);

    void OnSurfaceCreated(const char * vertexStr,const char * fragStr);

    void OnSurfaceChanged(int width, int height);

    void OnDrawFrame();

    static MyGLRenderContext *GetInstance();

    static void DestroyInstance();

    GLBaseSample *generateSample(int type);

    void init(int type);

    void UpdateTransformMatrix(jfloat rotateX, jfloat rotateY, jfloat scaleX, jfloat scaleY);

    void changeSamples(int num,const char * vertexStr,const char * fragStr);

    void changeZValue(int type,float zValue);

};

#endif //DEMOC_MYGLRENDERCONTEXT_H

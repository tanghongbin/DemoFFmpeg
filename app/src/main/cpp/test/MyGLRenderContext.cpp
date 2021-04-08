//
// Created by Admin on 2020/8/24.
//

#include "utils.h"
#include <GLES3/gl3.h>
#include <GLFBOSample.h>
#include <GLEglSample.h>
#include <CoordinateSystemSample.h>
#include <TextureMapSample.h>
#include <TextureCubeSample.h>
#include <TestFBOSample.h>
#include <LightSample.h>
#include <mutex>
#include <Model3DSample.h>
#include <DepthTestSample.h>
#include <BlendSample.h>
#include "CustomGLUtils.h"
#include "TriangleSample.h"
#include "MyGLRenderContext.h"
#include "TextureSample.h"
#include "GLYuvSample.h"
#include "GLVBOEBOSample.h"
#include "model/Model.h"

MyGLRenderContext *MyGLRenderContext::m_pContext = nullptr;

MyGLRenderContext::MyGLRenderContext() {
    m_Sample = nullptr;
}

GLBaseSample *MyGLRenderContext::generateSample(int type) {
    GLBaseSample *sample;
    switch (type) {
        case 1:
            sample = new TriangleSample();
            break;
        case 2:
            sample = new TextureSample();
            break;
        case 3:
            sample = new TextureMapSample();
            break;
        case 4:
            sample = new ParticlesSample();
            break;
        case 5:
            sample = new GLYuvSample();
            break;
        case 6:
            sample = new GLVBOEBOSample();
            break;
        case 7:
            sample = new GLFBOSample();
            break;
        case 8:
            sample = new CoordinateSystemSample();
            break;
        case 9:
            sample = new TextureCubeSample();
            break;
        case 10:
            sample = new TestFBOSample();
            break;
        case 11:
            sample = new LightSample();
            break;
        case 12:
            sample = new Model3DSample();
            break;
        case 13:
            sample = new DepthTestSample();
            break;
        case 14:
            sample = new BlendSample();
            break;
    }
    LOGCATE("create sample type:%d  point:%p",type,sample);
    return sample;
}

MyGLRenderContext::~MyGLRenderContext() {
    delete m_Sample;
    m_Sample = nullptr;
}


void MyGLRenderContext::SetImageData(int format, int width, int height, uint8_t *pData) {
    LOGCATE("MyGLRenderContext::SetImageData format=%d, width=%d, height=%d, pData=%p", format,
            width, height, pData);
    init(1);
    NativeImage nativeImage;
    nativeImage.format = format;
    nativeImage.width = width;
    nativeImage.height = height;
    nativeImage.ppPlane[0] = pData;

    switch (format) {
        case IMAGE_FORMAT_NV12:
        case IMAGE_FORMAT_NV21:
            nativeImage.ppPlane[1] = nativeImage.ppPlane[0] + width * height;
            break;
        case IMAGE_FORMAT_I420:
            nativeImage.ppPlane[1] = nativeImage.ppPlane[0] + width * height;
            nativeImage.ppPlane[2] = nativeImage.ppPlane[1] + width * height / 4;
            break;
        default:
            break;
    }

    m_Sample->loadImage(&nativeImage);

}

void MyGLRenderContext::OnSurfaceCreated(const char * vertexStr,const char * fragStr) {
    LOGCATE("OnSurfaceCreated");
    m_Sample->init(vertexStr,fragStr);
}

void MyGLRenderContext::OnSurfaceChanged(int width, int height) {
    LOGCATE("OnSurfaceChanged [w, h] = [%d, %d]", width, height);
    glViewport(0, 0, width, height);
    MyGLRenderContext::width = width;
    MyGLRenderContext::height = height;
    m_Sample->screenWidth = width;
    m_Sample->screenHeight = height;
}

void MyGLRenderContext::OnDrawFrame() {
    glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
    std::unique_lock<std::mutex> lock(my_mutex,std::defer_lock);
    if (lock.try_lock()){
        m_Sample->draw();
    } else {
        LOGCATE("no get lock");
    }
}

MyGLRenderContext *MyGLRenderContext::GetInstance() {
    if (m_pContext == nullptr) {
        m_pContext = new MyGLRenderContext();
    }
    return m_pContext;
}

void MyGLRenderContext::DestroyInstance() {
    LOGCATE("DestroyInstance");
    if (m_pContext) {
        GetInstance()->m_Sample->Destroy();
        delete m_pContext;
        m_pContext = nullptr;
    }


}

void MyGLRenderContext::init(int type) {
    if (m_Sample == nullptr) m_Sample = generateSample(type);
}

void MyGLRenderContext::UpdateTransformMatrix(jfloat rotateX, jfloat rotateY, jfloat scaleX,
                                              jfloat scaleY) {
    m_Sample->UpdateTransformMatrix(rotateX,rotateY,scaleX,scaleY);
}

void MyGLRenderContext::changeSamples(int num,const char * vertex,const char * fragmentStr) {
    LOGCATE("log type:%d vetex:\n\n%s \n\n%s",num,vertex,fragmentStr);
    std::unique_lock<std::mutex> lock(my_mutex);
    if (m_Sample){
        m_Sample->Destroy();
    }
    m_Sample = generateSample(num);
    m_Sample->init(vertex,fragmentStr);
    glCheckError("changeSamples");
    m_Sample->screenWidth = MyGLRenderContext::width;
    m_Sample->screenHeight = MyGLRenderContext::height;
}

void MyGLRenderContext::changeZValue(int type,float zValue){
    m_Sample->ChangeEyeZValue(type,zValue);
}




//
// Created by Admin on 2020/8/24.
//

#include "utils.h"
#include <GLES3/gl3.h>
#include <GLFBOSample.h>
#include <GLEglSample.h>
#include <CoordinateSystemSample.h>
#include <TextureMapSample.h>
#include "CustomGLUtils.h"
#include "TriangleSample.h"
#include "MyGLRenderContext.h"
#include "TextureSample.h"
#include "GLYuvSample.h"
#include "GLVBOEBOSample.h"

MyGLRenderContext *MyGLRenderContext::m_pContext = nullptr;

MyGLRenderContext::MyGLRenderContext() {
    m_Sample = generateSample(1);
}

GLBaseSample *MyGLRenderContext::generateSample(int type) {
    GLBaseSample *sample;
    switch (type) {
        case 1:
            sample = new TriangleSample();
            break;
        case 2:
            sample = new TextureMapSample();
            break;
        case 3:
            sample = new ParticlesSample();
            break;
        case 4:
            sample = new GLYuvSample();
            break;
        case 5:
            sample = new GLVBOEBOSample();
            break;
        case 6:
            sample = new GLFBOSample();
            break;
        case 7:
            sample = new CoordinateSystemSample();
            break;
    }
    LOGCATE("create sample:%p",sample);
    return sample;
}

MyGLRenderContext::~MyGLRenderContext() {
    delete m_Sample;
}


void MyGLRenderContext::SetImageData(int format, int width, int height, uint8_t *pData) {
    LOGCATE("MyGLRenderContext::SetImageData format=%d, width=%d, height=%d, pData=%p", format,
            width, height, pData);
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

void MyGLRenderContext::OnSurfaceCreated() {
    LOGCATE("OnSurfaceCreated");
    glClearColor(1.0f, 1.0f, 0.5f, 1.0f);
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
    m_Sample->init();
    m_Sample->draw();
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




//
// Created by thb on 1/23/21.
//

#include <EGL/eglplatform.h>
#include <EGL/egl.h>
#include <utils.h>
#include <CustomGLUtils.h>
#include <GLBaseSample.h>
#include <TriangleSample.h>
#include "EglContext.h"

void EglContext::init() {
    EGLint majorVersion;
    EGLint minorVersion;
    // 1.获取默认连接
    EGLDisplay display = eglGetDisplay(EGL_DEFAULT_DISPLAY);
    if (display == EGL_NO_DISPLAY){
        LOGCATE("can't find valid display");
        glCheckError("eglGetDisplay");
        return;
    }
    // 2.egl初始化
    if (!eglInitialize(display,&majorVersion,&minorVersion)){
        LOGCATE("can't init egl");
        glCheckError("eglInitialize");
        return;
    }
    LOGCATE("log majorversion:%d  minorversion:%d",majorVersion,minorVersion);

    // 3.选择渲染配置
    EGLint attributeLis[] = {
            EGL_RENDERABLE_TYPE,EGL_OPENGL_ES3_BIT,
            EGL_RED_SIZE,5,
            EGL_GREEN_SIZE,6,
            EGL_BLUE_SIZE,5,
            EGL_DEPTH_SIZE,1,
            EGL_NONE
    };
    EGLint maxConfig = 10;
    EGLConfig configs[maxConfig];
    EGLint chooseCount;
    if (!eglChooseConfig(display,attributeLis,configs,maxConfig,&chooseCount)){
        LOGCATE("can't choose config successfully");
        glCheckError("eglChooseConfig");
        return;
    }
    EGLConfig config = configs[0];
    // 4.创建屏幕外渲染窗口,还有个屏幕内渲染，但是缺少参数nativewindow
    EGLint surfaceAttributeList[] = {EGL_WIDTH,720,
                                     EGL_HEIGHT,1280,
                                     EGL_LARGEST_PBUFFER,EGL_TRUE,
                                     EGL_NONE};
    EGLSurface pBuffer = eglCreatePbufferSurface(display,config,surfaceAttributeList);
    if (pBuffer == EGL_NO_SURFACE){
        switch (eglGetError()) {
            case EGL_BAD_ALLOC:
                LOGCATE("not enough resouces available");
                break;
            case EGL_BAD_CONFIG:
                LOGCATE("config is invalid");
                break;
            case EGL_BAD_PARAMETER:
                LOGCATE("params is invalid");
                break;
            case EGL_BAD_MATCH:
                LOGCATE("check config or window attributes whther is valid");
                break;
        }
        return;
    }
    // 5.检查宽高，这里就不做了
    //6.创建opengl es 上下文，包含渲染信息等
    const EGLint createContextAttributes[] = {
            EGL_CONTEXT_CLIENT_VERSION,3,
            EGL_NONE
    };
    EGLContext eglContext = eglCreateContext(display,config,EGL_NO_CONTEXT,createContextAttributes);
    if (eglContext == EGL_NO_CONTEXT){
        switch (eglGetError()) {
            case EGL_BAD_CONFIG:
                LOGCATE("eglCreateContext bad config");
                break;
        }
        return;
    }
    //7.绑定并使用上下文
    if (!eglMakeCurrent(display,pBuffer,pBuffer,eglContext)){
        LOGCATE("make context failed");
        return;
    }
    TriangleSample * sample = new TriangleSample;
    sample->init("","");
    sample->draw();
    sample->Destroy();
    delete sample;

}


void EglContext::destroy() {

}

//
// Created by Admin on 2020/8/27.
//

#ifndef DEMOC_GLEGLSAMPLE_H
#define DEMOC_GLEGLSAMPLE_H


#include <EGL/egl.h>
#include "GLBaseSample.h"


class GLEglSample : public GLBaseSample {

private:
    EGLDisplay m_eglDisplay;
    EGLConfig  m_eglConf;
    EGLSurface m_eglSurface;
    EGLContext m_eglCtx;
    GLuint m_FboId,m_ImageTextureId;
    GLuint m_VaoIds[1];
    GLint m_TexSizeLoc;


public:

    void init(const char * vertexStr,const char * fragStr);

    void draw();

    void Destroy();

};


#endif //DEMOC_GLEGLSAMPLE_H

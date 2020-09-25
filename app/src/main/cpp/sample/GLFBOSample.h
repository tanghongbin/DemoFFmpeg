//
// Created by Admin on 2020/8/27.
//

#ifndef DEMOC_GLFBOSAMPLE_H
#define DEMOC_GLFBOSAMPLE_H


#include "GLBaseSample.h"

class GLFBOSample : public GLBaseSample {

private:
    GLuint m_ImageTextureId;
    GLuint m_FboTextureId;
    GLuint m_FboId;
    GLuint m_VaoIds[2];
    GLuint m_VboIds[4];
    GLint m_SamplerLoc;
    GLuint m_FboProgramObj;
    GLuint m_FboVertexShader;
    GLuint m_FboFragmentShader;
    GLint m_FboSamplerLoc;

public:
    void init();

    void draw();

    void Destroy();

    bool createFrameBufferObj();

};


#endif //DEMOC_GLFBOSAMPLE_H

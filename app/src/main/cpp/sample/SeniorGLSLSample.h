//
// Created by Admin on 2020/8/25.
//

#ifndef DEMOC_SENIORGLSLSAMPLE_H
#define DEMOC_SENIORGLSLSAMPLE_H

#include "GLES3/gl3.h"
#include "GLBaseSample.h"
#include <CustomGLUtils.h>

class SeniorGLSLSample : public GLBaseSample{

private:
    GLuint m_ProgramObj;
    GLuint vboIds[2];
    GLuint vaoIds[2];
    glm::mat4 m_MVPMatrix;

public:
    SeniorGLSLSample();

    ~SeniorGLSLSample();

    void init(const char * vertexStr,const char * fragStr);

    void init();

    void draw();

    void Destroy();

};

#endif //DEMOC_SENIORGLSLSAMPLE_H

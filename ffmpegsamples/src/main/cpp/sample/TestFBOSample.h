//
// Created by Admin on 2020/8/25.
//

#ifndef DEMOC_TestFBOSample_H
#define DEMOC_TestFBOSample_H

#include <GLES3/gl3.h>
#include "GLBaseSample.h"
#include <CustomGLUtils.h>

class TestFBOSample : public GLBaseSample{

private:
    GLuint m_ProgramObj;
    GLuint vboIds[3];
    GLuint vaoIds[2];
    glm::mat4 m_MVPMatrix;
public:
    TestFBOSample();

    ~TestFBOSample();

    void init(const char * vertexStr,const char * fragStr);

    void draw();

    void Destroy();

    void createFrameBuffer();
};

#endif //TestFBOSample

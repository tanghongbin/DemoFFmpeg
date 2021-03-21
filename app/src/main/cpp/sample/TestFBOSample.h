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
    GLuint m_FBO_ProgramObj;
    GLint m_FBO_SamplerLoc;
    GLuint vboIds[2] = {1, 2};
    GLuint vaoIds[2] = {3, 4};
    glm::mat4 m_MVPMatrix;

public:
    TestFBOSample();

    ~TestFBOSample();

    void init();

    void draw();

    void Destroy();
};

#endif //TestFBOSample

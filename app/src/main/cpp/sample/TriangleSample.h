//
// Created by Admin on 2020/8/25.
//

#ifndef DEMOC_TRIANGLESAMPLE_H
#define DEMOC_TRIANGLESAMPLE_H

#include "GLES3/gl3.h"
#include "GLBaseSample.h"
#include <CustomGLUtils.h>

class TriangleSample : public GLBaseSample{

private:
    GLuint m_ProgramObj;
    GLuint vboIds[2] = {1, 2};
    GLuint vaoIds[2] = {3, 4};
    glm::mat4 m_MVPMatrix;

public:
    TriangleSample();

    ~TriangleSample();

    void init();

    void draw();

    void Destroy();

    void renderWithOriginal(const GLfloat *vVertices, const GLfloat *colors1);

    void renderWithBuffer(const GLfloat *vVertices);

    void UpdateMVPMatrix(int angleX, int angleY, float scaleX, float scaleY);
};

#endif //DEMOC_TRIANGLESAMPLE_H

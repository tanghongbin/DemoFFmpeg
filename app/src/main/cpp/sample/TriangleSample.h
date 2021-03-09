//
// Created by Admin on 2020/8/25.
//

#ifndef DEMOC_TRIANGLESAMPLE_H
#define DEMOC_TRIANGLESAMPLE_H

#include "GLES3/gl3.h"
#include "GLBaseSample.h"

class TriangleSample : public GLBaseSample{

private:
    GLuint m_ProgramObj;
    GLuint vaoIds[2] = {1,2};
    GLint mvpLoc;

public:
    TriangleSample();

    ~TriangleSample();

    void init();

    void draw();

    void Destroy();

    void renderWithOriginal(const GLfloat *vVertices, const GLfloat *colors1) const;

    void renderWithBuffer(const GLfloat *vVertices) const;
};

#endif //DEMOC_TRIANGLESAMPLE_H

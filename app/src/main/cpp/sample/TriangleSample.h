//
// Created by Admin on 2020/8/25.
//

#ifndef DEMOC_TRIANGLESAMPLE_H
#define DEMOC_TRIANGLESAMPLE_H

#include "GLES3/gl3.h"
#include "GLBaseSample.h"

class TriangleSample : public GLBaseSample{

    GLuint m_ProgramObj;

public:
    TriangleSample();

    ~TriangleSample();

    void init();

    void draw();

    void Destroy();

};

#endif //DEMOC_TRIANGLESAMPLE_H

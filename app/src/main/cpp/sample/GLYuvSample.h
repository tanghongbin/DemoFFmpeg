//
// Created by Admin on 2020/8/26.
//


#ifndef DEMOC_GLYUVSAMPLE_H
#define DEMOC_GLYUVSAMPLE_H

#include <ext.hpp>
#include "GLBaseSample.h"

class GLYuvSample : public GLBaseSample {

private:
    GLint m_ySamplerLoc,m_uvSamplerLoc;
    GLuint m_yTextureId,m_uvTextureId;

public:
    void init();

    void draw();

    void Destroy();

};


#endif //DEMOC_GLYUVSAMPLE_H

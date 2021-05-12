//
// Created by Admin on 2020/8/26.
//

#ifndef DEMOC_GLVBOEBOSAMPLE_H
#define DEMOC_GLVBOEBOSAMPLE_H


#include "GLBaseSample.h"

class GLVBOEBOSample : public GLBaseSample {

private:
    GLuint* m_VboIds;

public:

    void init(const char * vertexStr,const char * fragStr);

    void draw();

    void Destroy();

};


#endif //DEMOC_GLVBOEBOSAMPLE_H

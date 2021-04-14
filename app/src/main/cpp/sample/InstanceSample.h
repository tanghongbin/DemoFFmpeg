//
// Created by Admin on 2020/8/25.
//

#ifndef DEMOC_INSTANCESAMPLESAMPLE_H
#define DEMOC_INSTANCESAMPLESAMPLE_H


#include <ImageDef.h>
#include <stb_image.h>
#include <detail/type_mat.hpp>
#include <gtc/type_ptr.hpp> // glm::value_ptr
#include "GLBaseSample.h"

class InstanceSample : public GLBaseSample {

public:

    void init(const char * vertexStr,const char * fragStr);

    void draw();

    void Destroy();

private:
    stbi_uc * imageData = nullptr;
    GLuint vboIds[4];
    GLuint vaoIds[4];
    int64_t second;


};


#endif //DEMOC_INSTANCESAMPLESAMPLE_H

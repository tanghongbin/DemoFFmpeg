//
// Created by Admin on 2020/8/25.
//

#ifndef DEMOC_BLENDSAMPLE_H
#define DEMOC_BLENDSAMPLE_H


#include <ImageDef.h>
#include <stb_image.h>
#include <detail/type_mat.hpp>
#include <gtc/type_ptr.hpp> // glm::value_ptr
#include "GLBaseSample.h"

class BlendSample : public GLBaseSample {

public:

    void init(const char * vertexStr,const char * fragStr);

    void draw();

    void Destroy();

private:
    stbi_uc * imageData = nullptr;
    glm::mat4 model;
    glm::mat4 view;
    glm::mat4 projection;
    GLuint vboIds[2];


    void createMvp();
};


#endif //DEMOC_BLENDSAMPLE_H

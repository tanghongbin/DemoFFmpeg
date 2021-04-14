//
// Created by Admin on 2020/8/25.
//

#ifndef DEMOC_INSTANCECOPYSAMPLESAMPLE_H
#define DEMOC_INSTANCECOPYSAMPLESAMPLE_H


#include <ImageDef.h>
#include <stb_image.h>
#include <detail/type_mat.hpp>
#include <gtc/type_ptr.hpp> // glm::value_ptr
#include <model/model.h>
#include "GLBaseSample.h"

class InstanceCopySample : public GLBaseSample {

public:

    void init(const char * vertexStr,const char * fragStr);

    void draw();

    void Destroy();

private:
    Model * mRock;
    glm::mat4 *modelMatrices;
    unsigned int amount = 100;
    GLuint bigDataVbo;


};


#endif //DEMOC_INSTANCECOPYSAMPLESAMPLE_H

//
// Created by Admin on 2020/8/25.
//

#ifndef DEMOC_INSTANCESAMPLESAMPLE_H
#define DEMOC_INSTANCESAMPLESAMPLE_H


#include <ImageDef.h>
#include <stb_image.h>
#include <detail/type_mat.hpp>
#include <gtc/type_ptr.hpp> // glm::value_ptr
#include <model/model.h>
#include "GLBaseSample.h"

class InstanceSample : public GLBaseSample {

public:

    void init(const char * vertexStr,const char * fragStr);

    void draw();

    void Destroy();

private:
    Model * mPlanModel;
    Model * mRock;
    glm::mat4 *modelMatrices;
    unsigned int amount = 10000;
    GLuint bigDataVbo;
    Shader* mRockShader;


};


#endif //DEMOC_INSTANCESAMPLESAMPLE_H

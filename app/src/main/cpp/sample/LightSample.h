//
// Created by Admin on 2020/8/25.
//

#ifndef DEMOC_LightSampleSAMPLE_H
#define DEMOC_LightSampleSAMPLE_H


#include <ImageDef.h>
#include <stb_image.h>
#include <detail/type_mat.hpp>
#include <gtc/type_ptr.hpp> // glm::value_ptr
#include "GLBaseSample.h"

class LightSample : public GLBaseSample {

public:

    void init(const char * vertexStr,const char * fragStr);

    void draw();

    void Destroy();

    void UpdateTransformMatrix(float rotateX, float rotateY, float scaleX,
                               float scaleY);

private:
    stbi_uc * imageData = nullptr;
    glm::mat4 model;
    glm::mat4 view;
    glm::mat4 projection;
    glm::mat4 mMvpMatrix;
    GLuint vboIds[2];
    GLuint lightVao;
    GLint lightProgram;
    int64_t second;
    GLuint mLightTexture[10];

    int m_AngleX = 0;
    int m_AngleY = 0 ;
    float m_ScaleX = 0.0f;
    float m_ScaleY = 0.0f;

    void UpdateMvp( int angleX, int angleY, float ratio);
};


#endif //DEMOC_TEXTURESAMPLE_H

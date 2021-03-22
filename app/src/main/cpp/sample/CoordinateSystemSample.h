//
// Created by Admin on 2020/8/28.
//

#ifndef DEMOC_COORDINATESYSTEMSAMPLE_H
#define DEMOC_COORDINATESYSTEMSAMPLE_H


#include "GLBaseSample.h"
#include "glm.hpp"

class CoordinateSystemSample : public GLBaseSample {

public:
    CoordinateSystemSample();

    ~CoordinateSystemSample();

    void init(const char * vertexStr,const char * fragStr);

    void draw();

    void Destroy();

    void UpdateTransformMatrix(float rotateX, float rotateY, float scaleX, float scaleY);

    void UpdateMVPMatrix(glm::mat4 &mvpMatrix, int angleX, int angleY, float ratio);

private:
    GLuint m_TextureId;
    GLint m_SamplerLoc;
    GLint m_MVPMatLoc;
    GLuint m_VaoId;
    GLuint m_VboIds[3];
    NativeImage m_RenderImage;
    glm::mat4 m_MVPMatrix;

    int m_AngleX;
    int m_AngleY;
    float m_ScaleX;
    float m_ScaleY;
};


#endif //DEMOC_COORDINATESYSTEMSAMPLE_H

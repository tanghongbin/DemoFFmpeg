//
// Created by Admin on 2020/8/25.
//

#ifndef DEMOC_Model3DSAMPLE_H
#define DEMOC_Model3DSAMPLE_H

#include "GLES3/gl3.h"
#include "GLBaseSample.h"
#include <CustomGLUtils.h>
#include "model/model.h"
#include "model/shader.h"


class Model3DSample : public GLBaseSample{

private:
    GLuint m_ProgramObj;
    GLuint vboIds[2];
    GLuint vaoIds[2];
    glm::mat4 m_MVPMatrix;
    glm::mat4 m_MvpModel;
    Shader* shader;
    Model* mModel;


public:

    bool ifNeedClear = true;

    Model3DSample();

    ~Model3DSample();

    void init(const char * vertexStr,const char * fragStr);

    void init();

    void draw();

    void Destroy();

    void UpdateMVPMatrix(glm::mat4 &mvpMatrix, int angleX, int angleY, float ratio);

    void UpdateTransformMatrix(float rotateX, float rotateY, float scaleX, float scaleY) ;

};

#endif //DEMOC_Model3DSAMPLE_H

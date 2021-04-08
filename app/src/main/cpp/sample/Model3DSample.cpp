//
// Created by Admin on 2020/8/24.
//

#include <camera.h>
#include "Model3DSample.h"


Model3DSample::Model3DSample() {

}

Model3DSample::~Model3DSample() {
    shader->Destroy();
    delete mModel;
    delete shader;
}

void Model3DSample::init(const char * vShaderStr,const char * fShaderStr) {
    shader = new Shader(vShaderStr,fShaderStr);
    const char * path = "/storage/emulated/0/ffmpegtest/3d_obj/apple/Apricot_02_hi_poly.obj";
    const char * path2 = "/storage/emulated/0/ffmpegtest/3d_obj/naosuit/nanosuit.obj";
    mModel = new Model(path2);
}

void Model3DSample::draw() {

    if (shader->ID == 0)
        return;

    glClear(GL_STENCIL_BUFFER_BIT | GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glClearColor(1.0, 1.0, 1.0, 1.0);

    UpdateMVPMatrix(m_MVPMatrix, m_AngleX, m_AngleY, (float)screenWidth / screenHeight);
    // Use the program object
    glEnable(GL_DEPTH_TEST);
    shader->use();
    glm::vec3 lightPos = glm::vec3( 0.0,  1.0f,  2.0f);
    glm::vec3 cameraPos = glm::vec3(-3,0.0f,3.0);
    Camera camera(cameraPos);
    shader->setVec3("lightColor",glm::vec3(1.0,1.0,1.0));
    shader->setVec3("lightPos", glm::vec3 (0.0,0.0,mModel->GetMaxViewDistance()));
    shader->setVec3("viewPos",glm::vec3(0, 0, mModel->GetMaxViewDistance()));
    shader->setMat4("m_MVPMatrix", m_MVPMatrix);
    shader->setMat4("model", m_MvpModel);
    shader->setFloat("material.shininess",8.0);
    shader->setFloat("material.ambient",0.3);
    shader->setFloat("material.diffuse",0.8);
    shader->setFloat("material.specular",0.3);
    mModel->Draw(*shader);
}


void Model3DSample::Destroy() {
    glDeleteVertexArrays(1,&vboIds[0]);
}


void Model3DSample::UpdateMVPMatrix(glm::mat4 &mvpMatrix, int angleX, int angleY, float ratio)
{
//    LOGCATE("Model3DSample::UpdateMVPMatrix angleX = %d, angleY = %d, ratio = %f", angleX, angleY, ratio);
    angleX = angleX % 360;
    angleY = angleY % 360;

    //转化为弧度角
    float radiansX = static_cast<float>(MATH_PI / 180.0f * angleX);
    float radiansY = static_cast<float>(MATH_PI / 180.0f * angleY);


    // Projection matrix
    //glm::mat4 Projection = glm::ortho(-ratio, ratio, -1.0f, 1.0f, 0.1f, 100.0f);
    glm::mat4 Projection = glm::frustum(-ratio, ratio, -1.0f, 1.0f, 1.0f, mModel->GetMaxViewDistance() * 4);
    //glm::mat4 Projection = glm::perspective(45.0f,ratio, 0.1f,100.f);

    // View matrix
    glm::mat4 View = glm::lookAt(
            glm::vec3(0, 0, mModel->GetMaxViewDistance() * 1.8f), // Camera is at (0,0,1), in World Space
            glm::vec3(0, 0, 0), // and looks at the origin
            glm::vec3(0, 1, 0)  // Head is up (set to 0,-1,0 to look upside-down)
    );

    // Model matrix
    glm::mat4 Model = glm::mat4(1.0f);
    Model = glm::scale(Model, glm::vec3(m_ScaleX, m_ScaleY, 1.0f));
    Model = glm::rotate(Model, radiansX, glm::vec3(1.0f, 0.0f, 0.0f));
    Model = glm::rotate(Model, radiansY, glm::vec3(0.0f, 1.0f, 0.0f));
    Model = glm::translate(Model, -mModel->GetAdjustModelPosVec());
    m_MvpModel = Model;
    mvpMatrix = Projection * View * Model;
}


void Model3DSample::UpdateTransformMatrix(float rotateX, float rotateY, float scaleX, float scaleY)
{

//    GLSampleBase::UpdateTransformMatrix(rotateX, rotateY, scaleX, scaleY);
    m_AngleX = static_cast<int>(rotateX);
    m_AngleY = static_cast<int>(rotateY);
    m_ScaleX = scaleX;
    m_ScaleY = scaleY;


//    LOGCATE("BasicLightingSample::UpdateMVPMatrix angleX = %d, angleY = %d, ratio = %f", angleX,
//            angleY, ratio);
}
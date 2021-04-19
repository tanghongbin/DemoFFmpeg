//
// Created by Admin on 2020/8/25.
//


#include "InstanceSample.h"
#include <GLES3/gl3.h>
#include <camera.h>
#include <OpenGLImageDef.h>
#include <opencv2/videostab/motion_stabilizing.hpp>
#include "stb_image.h"
#include "CustomGLUtils.h"
#include "ImageDef.h"

void InstanceSample::init(const char * vShaderStr,const char * fShaderStr) {

    mShader = new Shader(vShaderStr,fShaderStr);
    mRockShader = new Shader(readGLSLStrFromFile("instance/rockvetex.glsl").c_str(),
                             readGLSLStrFromFile("instance/fragment.glsl").c_str());

//    LOGCATE("log rockshader:%p shader:%p",mRockShader,mShader);
    mPlanModel = new Model(getModel3DPath("planet/planet.obj"));
    mRock = new Model(getModel3DPath("rock/rock.obj"));

    modelMatrices = new glm::mat4[amount];
    float radius = 50.0;
    float offset = 2.5f;
    for(unsigned int i = 0; i < amount; i++)
    {
        glm::mat4 model;
        // 1. 位移：分布在半径为 'radius' 的圆形上，偏移的范围是 [-offset, offset]
        float angle = (float)i / (float)amount * 360.0f;
        float displacement = (rand() % (int)(2 * offset * 100)) / 100.0f - offset;
        float x = sin(angle) * radius + displacement;
        displacement = (rand() % (int)(2 * offset * 100)) / 100.0f - offset;
        float y = displacement * 0.4f; // 让行星带的高度比x和z的宽度要小
        displacement = (rand() % (int)(2 * offset * 100)) / 100.0f - offset;
        float z = cos(angle) * radius + displacement;
        model = glm::translate(model, glm::vec3(x, y, z));

        // 2. 缩放：在 0.05 和 0.25f 之间缩放
        float scale = (rand() % 20) / 100.0f + 0.05;
//        float scale = 0.1;
        model = glm::scale(model, glm::vec3(scale));
//        model = glm::scale(model, glm::vec3(0.001,0.001,0.001));

        // 3. 旋转：绕着一个（半）随机选择的旋转轴向量进行随机的旋转
        float rotAngle = (rand() % 360);
        model = glm::rotate(model, rotAngle, glm::vec3(0.4f, 0.6f, 0.8f));

        // 4. 添加到矩阵的数组中
        modelMatrices[i] = model;
    }

    glGenBuffers(1,&bigDataVbo);
    glBindBuffer(GL_ARRAY_BUFFER,bigDataVbo);
    glBufferData(GL_ARRAY_BUFFER, amount * sizeof(glm::mat4), &modelMatrices[0], GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER,0);
    for (int i = 0; i < mRock->meshes.size(); ++i) {
        glBindVertexArray(mRock->meshes[i].Vao);
        mRock->meshes[i].bindBufferInternal();
        GLsizei vec4Size = sizeof(glm::vec4);
        GLsizei strideSize = 4 * vec4Size;
        glBindBuffer(GL_ARRAY_BUFFER, bigDataVbo);
        glVertexAttribPointer(3,4,GL_FLOAT,GL_FALSE,strideSize,(void *)0);
        glVertexAttribPointer(4,4,GL_FLOAT,GL_FALSE,strideSize,(void *)(vec4Size));
        glVertexAttribPointer(5,4,GL_FLOAT,GL_FALSE,strideSize,(void *)(2 * vec4Size));
        glVertexAttribPointer(6,4,GL_FLOAT,GL_FALSE,strideSize,(void *)(3 * vec4Size));

        glEnableVertexAttribArray(3);
        glEnableVertexAttribArray(4);
        glEnableVertexAttribArray(5);
        glEnableVertexAttribArray(6);

        glVertexAttribDivisor(3,1);
        glVertexAttribDivisor(4,1);
        glVertexAttribDivisor(5,1);
        glVertexAttribDivisor(6,1);
        glBindVertexArray(0);
    }
    glBindBuffer(GL_ARRAY_BUFFER,0);
    LOGCATE("all bind over");
}


void InstanceSample::draw() {

    glClear(GL_STENCIL_BUFFER_BIT | GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glClearColor(1.0, 1.0, 1.0, 1.0);
    glEnable(GL_DEPTH_TEST);
    int64_t startTime = GetSysCurrentTime();

//    LOGCATE("planet has over:%p shader:%p",mRockShader,mShader);

    mShader->use();
    UpdateMvp();
//    mBaseModel = glm::translate(mBaseModel, glm::vec3(0.0f, -3.0f, 0.0f));
    const glm::mat4 &localModel = glm::scale(mBaseModel, glm::vec3(0.2f, 0.2f, 0.2f));
    const glm::mat4 &mvpMatrix = mBaseProjection * mBaseView * localModel;
    mShader->setMat4("model",localModel);
    mShader->setMat4("m_MVPMatrix",mvpMatrix);
//    LOGCATE("planet has over:%p shader:%p",mRockShader,mShader);
    mPlanModel->Draw(mShader);

    mRockShader->use();
//    LOGCATE("mRockShader has used");
    mRockShader->setMat4("view",mBaseView);
    mRockShader->setMat4("projection",mBaseProjection);
    mShader->setInt("texture_diffuse1", 0);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, mRock ->textures_loaded[0].id);
    // 绘制小行星
    for(unsigned int i = 0; i < mRock->meshes.size(); i++)
    {
        Mesh &mesh = mRock->meshes[i];
        glBindVertexArray(mesh.Vao);
        glDrawElementsInstanced(GL_TRIANGLES,mesh.indices.size(),GL_UNSIGNED_INT,0,amount);
    }
    LOGCATE("log totally cost:%lld",(GetSysCurrentTime() - startTime));

}

void InstanceSample::Destroy()
{
    if (m_ProgramObj)
    {

        delete modelMatrices;
        mRockShader->Destroy();
        delete mRockShader;
        mRockShader = 0;

        glDeleteBuffers(1,&bigDataVbo);
        glDeleteTextures(1,&m_TextureId);
        glDeleteProgram(m_ProgramObj);
        mPlanModel->Destroy();
        delete mPlanModel;
        mRock->Destroy();
        delete mRock;
    }

}

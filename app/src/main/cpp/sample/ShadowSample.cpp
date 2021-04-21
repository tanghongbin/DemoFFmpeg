//
// Created by Admin on 2020/8/24.
//

#include "ShadowSample.h"


ShadowSample::ShadowSample() {

}

ShadowSample::~ShadowSample() {
    if (m_ProgramObj) {
        glDeleteProgram(m_ProgramObj);
    }

}

void ShadowSample::init(const char * vShaderStr,const char * fShaderStr) {


    mShader = new Shader(vShaderStr,fShaderStr);
    lightShader = new Shader(vShaderStr,readGLSLStrFromFile("shadow/lightfragment.glsl").c_str());
    shadowShader = new Shader(readGLSLStrFromFile("shadow/shadowvetex.glsl").c_str(),
            readGLSLStrFromFile("shadow/shadowfragment.glsl").c_str());
    // bind vao,vbo
    float verticesInner[] = {
            // position,texcoords,normal
            -0.5f, -0.5f, -0.5f,  0.0f, 0.0f, 0.0f,  0.0f, -1.0f,
            0.5f, -0.5f, -0.5f,  1.0f, 0.0f,  0.0f,  0.0f, -1.0f,
            0.5f,  0.5f, -0.5f,  1.0f, 1.0f,  0.0f,  0.0f, -1.0f,
            0.5f,  0.5f, -0.5f,  1.0f, 1.0f,  0.0f,  0.0f, -1.0f,
            -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,  0.0f,  0.0f, -1.0f,
            -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,  0.0f,  0.0f, -1.0f,

            -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,   0.0f,  0.0f,  1.0f,
            0.5f, -0.5f,  0.5f,  1.0f, 0.0f,   0.0f,  0.0f,  1.0f,
            0.5f,  0.5f,  0.5f,  1.0f, 1.0f,   0.0f,  0.0f,  1.0f,
            0.5f,  0.5f,  0.5f,  1.0f, 1.0f,   0.0f,  0.0f,  1.0f,
            -0.5f,  0.5f,  0.5f,  0.0f, 1.0f,   0.0f,  0.0f,  1.0f,
            -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,   0.0f,  0.0f,  1.0f,

            -0.5f,  0.5f,  0.5f,  1.0f, 0.0f,  1.0f,  0.0f,  0.0f,
            -0.5f,  0.5f, -0.5f,  1.0f, 1.0f,  1.0f,  0.0f,  0.0f,
            -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,  1.0f,  0.0f,  0.0f,
            -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,  1.0f,  0.0f,  0.0f,
            -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,  1.0f,  0.0f,  0.0f,
            -0.5f,  0.5f,  0.5f,  1.0f, 0.0f,  1.0f,  0.0f,  0.0f,

            0.5f,  0.5f,  0.5f,  1.0f, 0.0f,  1.0f,  0.0f,  0.0f,
            0.5f,  0.5f, -0.5f,  1.0f, 1.0f,  1.0f,  0.0f,  0.0f,
            0.5f, -0.5f, -0.5f,  0.0f, 1.0f,  1.0f,  0.0f,  0.0f,
            0.5f, -0.5f, -0.5f,  0.0f, 1.0f,  1.0f,  0.0f,  0.0f,
            0.5f, -0.5f,  0.5f,  0.0f, 0.0f,  1.0f,  0.0f,  0.0f,
            0.5f,  0.5f,  0.5f,  1.0f, 0.0f,  1.0f,  0.0f,  0.0f,

            -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,  0.0f, -1.0f,  0.0f,
            0.5f, -0.5f, -0.5f,  1.0f, 1.0f,  0.0f, -1.0f,  0.0f,
            0.5f, -0.5f,  0.5f,  1.0f, 0.0f,  0.0f, -1.0f,  0.0f,
            0.5f, -0.5f,  0.5f,  1.0f, 0.0f,  0.0f, -1.0f,  0.0f,
            -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,  0.0f, -1.0f,  0.0f,
            -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,  0.0f, -1.0f,  0.0f,

            -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,   0.0f,  1.0f,  0.0f,
            0.5f,  0.5f, -0.5f,  1.0f, 1.0f,   0.0f,  1.0f,  0.0f,
            0.5f,  0.5f,  0.5f,  1.0f, 0.0f,   0.0f,  1.0f,  0.0f,
            0.5f,  0.5f,  0.5f,  1.0f, 0.0f,   0.0f,  1.0f,  0.0f,
            -0.5f,  0.5f,  0.5f,  0.0f, 0.0f,   0.0f,  1.0f,  0.0f,
            -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,   0.0f,  1.0f,  0.0f
    };
    glGenBuffers(4, vboIds);
    glBindBuffer(GL_ARRAY_BUFFER, vboIds[0]);
    glBufferData(GL_ARRAY_BUFFER,sizeof(verticesInner),verticesInner,GL_STATIC_DRAW);

    glGenVertexArrays(4,vaoIds);
    glBindVertexArray(vaoIds[0]);

    glBindBuffer(GL_ARRAY_BUFFER, vboIds[0]);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0,3,GL_FLOAT,GL_FALSE,sizeof(GLfloat) * 8,(const void *)0);

    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1,2,GL_FLOAT,GL_FALSE,sizeof(GLfloat) * 8,(const void *)(sizeof(GLfloat) * 3));

    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2,3,GL_FLOAT,GL_FALSE,sizeof(GLfloat) * 8,(const void *)(sizeof(GLfloat) * 5));
    glBindVertexArray(0);

    LoadImageInfo imageInfo;
    imageInfo.loadImage("/storage/emulated/0/ffmpegtest/filterImg/test_box.png");
    glGenTextures(1,&m_TextureId);
    glBindTexture(GL_TEXTURE_2D,m_TextureId);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    imageInfo.uploadImageTex2D();
    glBindTexture(GL_TEXTURE_2D,0);

    // 地板

    // bind vao,vbo
    float verticesFloor[] = {
            // position,texcoords,normal
//            25.0f, -0.5f,  25.0f, 1.0f,  0.0f, 0.0f, 1.0f, 0.0f,
//            -25.0f, -0.5f,  25.0f,  0.0f,  0.0f, 0.0f, 1.0f, 0.0f,
//            -25.0f, -0.5f, -25.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f,
//
//            25.0f, -0.5f,  25.0f,  1.0f,  0.0f, 0.0f, 1.0f, 0.0f,
//            -25.0f, -0.5f, -25.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f,
//            25.0f, -0.5f, -25.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f,

            -25.0, -2.0, -25.0,  0.0f, 0.0f,  0.0f,  0.0f, 1.0f,
            -25.0f, -2.0, 25.0,  0.0f, 25.0f,  0.0f,  0.0f, 1.0f,
            25.0f,  -2.0, 25.0,  25.0f, 25.0f,  0.0f,  0.0f, 1.0f,

            25.0f,  -2.0, 25.0,  25.0f, 25.0f,  0.0f,  0.0f, 1.0f,
            25.0,  -2.0, -25.0,  25.0f, 0.0f,  0.0f,  0.0f, 1.0f,
            -25.0, -2.0, -25.0,  0.0f, 0.0f, 0.0f,  0.0f, 1.0f,


    };
    glBindBuffer(GL_ARRAY_BUFFER, vboIds[1]);
    glBufferData(GL_ARRAY_BUFFER,sizeof(verticesFloor),verticesFloor,GL_STATIC_DRAW);

    glBindVertexArray(vaoIds[1]);
    glBindBuffer(GL_ARRAY_BUFFER, vboIds[1]);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0,3,GL_FLOAT,GL_FALSE,sizeof(GLfloat) * 8,(const void *)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1,2,GL_FLOAT,GL_FALSE,sizeof(GLfloat) * 8,(const void *)(sizeof(GLfloat) * 3));
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2,3,GL_FLOAT,GL_FALSE,sizeof(GLfloat) * 8,(const void *)(sizeof(GLfloat) * 5));

    glBindVertexArray(0);

    LoadImageInfo imageInfoFloor;
    imageInfoFloor.loadImage("/storage/emulated/0/ffmpegtest/filterImg/wood.png");
    glGenTextures(2,mTextureIds);
    glBindTexture(GL_TEXTURE_2D,mTextureIds[0]);
    imageInfoFloor.uploadImageTex2D();
    glGenerateMipmap(GL_TEXTURE_2D);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT );
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT );
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glBindTexture(GL_TEXTURE_2D,0);
    generateFbo();

    float normalVetex[] = {
            -1.0f, -1.0f, -0.0f,  0.0f, 0.0f, 0.0f,  0.0f, -1.0f,
            1.0f, -1.0f, -0.0f,  1.0f, 0.0f,  0.0f,  0.0f, -1.0f,
            1.0f,  1.0f, -0.0f,  1.0f, 1.0f,  0.0f,  0.0f, -1.0f,
            1.0f,  1.0f, -0.0f,  1.0f, 1.0f,  0.0f,  0.0f, -1.0f,
            -1.0f,  1.0f, -0.0f,  0.0f, 1.0f,  0.0f,  0.0f, -1.0f,
            -1.0f, -1.0f, -0.0f,  0.0f, 0.0f,  0.0f,  0.0f, -1.0f,
    };
    glBindBuffer(GL_ARRAY_BUFFER, vboIds[2]);
    glBufferData(GL_ARRAY_BUFFER,sizeof(normalVetex),normalVetex,GL_STATIC_DRAW);

    glBindVertexArray(vaoIds[2]);
    glBindBuffer(GL_ARRAY_BUFFER, vboIds[2]);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0,3,GL_FLOAT,GL_FALSE,sizeof(GLfloat) * 8,(const void *)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1,2,GL_FLOAT,GL_FALSE,sizeof(GLfloat) * 8,(const void *)(sizeof(GLfloat) * 3));
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2,3,GL_FLOAT,GL_FALSE,sizeof(GLfloat) * 8,(const void *)(sizeof(GLfloat) * 5));

    glBindVertexArray(0);
}

void ShadowSample::draw() {

    if (!mShader->ID) return;

    glClear(GL_STENCIL_BUFFER_BIT | GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glClearColor(1.0, 1.0, 1.0, 1.0);

    glViewport(0,0,SHADOW_WIDTH,SHADOW_HEIGHT);
    glBindFramebuffer(GL_FRAMEBUFFER,m_fboId);
    glClear(GL_DEPTH_BUFFER_BIT);
    ConfigureShaderAndMatrices();
    renderScene(shadowShader);
    glBindFramebuffer(GL_FRAMEBUFFER,0);
//
    glViewport(0,0,SHADOW_WIDTH,SHADOW_HEIGHT);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    mShader->use();
    glBindVertexArray(vaoIds[2]);
    mShader->setInt("type",2);
    mShader->setInt("material.diffuse",3);
    mShader->setInt("isNormalTexture",1);
    glActiveTexture(GL_TEXTURE3);
    glBindTexture(GL_TEXTURE_2D,depthTexture);
    glDrawArrays(GL_TRIANGLES,0,6);
//    renderScene(mShader);
}

/**
 * 渲染场景
 */
void ShadowSample::renderScene(Shader *mShader) {
    glEnable(GL_DEPTH_TEST);
    // Use the program object
    UpdateMvp();
    // 画灯
    glm::vec3 lightColor = glm::vec3 (1.0,1.0,1.0);
    glm::vec3 lightPos = glm::vec3(0.0f,  3.0f, 0.0f);
//    lightShader->use();
//    glBindVertexArray(vaoIds[0]);
//    glm::mat4 resultModel;
//    float angle = 15.0f;
//    resultModel = glm::translate(mBaseModel, lightPos);
//    resultModel = glm::scale(resultModel,glm::vec3(0.3f,0.3f,0.3f));
//    resultModel = glm::rotate(resultModel, glm::radians(angle), glm::vec3(1.0f, 0.3f, 0.5f));
//    lightShader->setMat4("model", resultModel);
//    lightShader->setVec3("temparyLightColor",lightColor);
//    lightShader->setMat4("mvp", mBaseProjection * mBaseView * resultModel);
//    glDrawArrays(GL_TRIANGLES, 0, 36);

/***------阴影矩阵---------***/

    GLfloat near_plane = 1.0f, far_plane = 7.5f;
    glm::mat4 lightProjection = glm::ortho(-10.0f, 10.0f, -10.0f, 10.0f, near_plane, far_plane);
    glm::mat4 lightView = glm::lookAt(glm::vec3(-2.0f, 4.0f, -1.0f), glm::vec3(0.0f, 0.0f, 0.0f),
            glm::vec3(0.0f, 1.0f, 0.0f));
    mBaseProjection = lightProjection;
    mBaseView = lightView;
    mBaseMvpMatrix = lightProjection * lightView * mBaseModel;
/***------阴影矩阵---------***/

    // 画矩形
    mShader->use();
    mShader->setMat4("mvp", mBaseMvpMatrix);
    mShader->setMat4("model", mBaseModel);

    mShader->setFloat("material.shininess", 8.0);
    mShader->setInt("material.ambient", 0);
    mShader->setInt("material.diffuse", 0);
    mShader->setInt("material.specular", 0);

    mShader->setVec3("pointLight.lightColor", lightColor);
    mShader->setVec3("pointLight.position", lightPos);
    mShader->setVec3("pointLight.ambient", 0.05f, 0.05f, 0.05f);
    mShader->setVec3("pointLight.diffuse", 0.8f, 0.8f, 0.8f);
    mShader->setVec3("pointLight.specular", 1.0f, 1.0f, 1.0f);
    mShader->setFloat("pointLight.constant", 1.0f);
    mShader->setFloat("pointLight.linear", 0.09);
    mShader->setFloat("pointLight.quadratic", 0.032);
    mShader->setVec3("viewPos", glm::vec3(0.0, 0.0, 3.0));


    glBindVertexArray(vaoIds[0]);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, m_TextureId);
    mShader->setInt("type", 0);
    glDrawArrays(GL_TRIANGLES,0,36);
    glBindVertexArray(0);

    // 画地板
    glm::mat4 model = mBaseModel;
    model = glm::translate(model,glm::vec3(0.0,0.0,0.0f));
    glm::mat4 mvp = mBaseProjection * mBaseView * model;
    mShader->setMat4("mvp", mvp);
    mShader->setMat4("model", model);
    glBindVertexArray(vaoIds[1]);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, mTextureIds[0]);
    mShader->setInt("material.ambient", 1);
    mShader->setInt("material.diffuse", 1);
    mShader->setInt("material.specular", 1);
    mShader->setInt("type", 1);
    glDrawArrays(GL_TRIANGLES,0,6);
}


void ShadowSample::generateFbo() {
    glGenFramebuffers(1,&m_fboId);

    glGenTextures(1, &depthTexture);
    glBindTexture(GL_TEXTURE_2D, depthTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT,
                 SHADOW_WIDTH, SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    glBindFramebuffer(GL_FRAMEBUFFER,m_fboId);
    glFramebufferTexture2D(GL_FRAMEBUFFER,GL_DEPTH_ATTACHMENT,GL_TEXTURE_2D,depthTexture,0);
    glDrawBuffers(0,GL_NONE);
    glReadBuffer(GL_NONE);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void ShadowSample::ConfigureShaderAndMatrices() {

}


void ShadowSample::Destroy() {
    glDeleteVertexArrays(4,vaoIds);
    glDeleteBuffers(4,vboIds);
    glDeleteFramebuffers(1,&m_fboId);
    if (lightShader){
        lightShader->Destroy();
        delete lightShader;
        lightShader = 0;
        shadowShader->Destroy();
        delete shadowShader;
        shadowShader = 0;
    }
}




//
// Created by Admin on 2020/8/24.
//

#include "ShadowCubeSample.h"

// 点阴影暂时不会生效
ShadowCubeSample::ShadowCubeSample() {

}

ShadowCubeSample::~ShadowCubeSample() {
    if (m_ProgramObj) {
        glDeleteProgram(m_ProgramObj);
    }
}

void ShadowCubeSample::init(const char * vShaderStr,const char * fShaderStr) {


    mShader = new Shader(vShaderStr,fShaderStr);
    mLightShader = new Shader(vShaderStr, readGLSLStrFromFile("shadowcube/lightfragment.glsl").c_str());
    mShadowShader = new Shader(readGLSLStrFromFile("shadowcube/shadowvetex.glsl").c_str(),
                               readGLSLStrFromFile("shadowcube/shadowfragment.glsl").c_str());

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
            -25.0, -0.5, -25.0,  0.0f, 0.0f,  0.0f,  0.0f, 1.0f,
            -25.0f, -0.5, 25.0,  0.0f, 25.0f,  0.0f,  0.0f, 1.0f,
            25.0f, -0.5, 25.0,  25.0f, 25.0f,  0.0f,  0.0f, 1.0f,

            25.0f,  -0.5, 25.0,  25.0f, 25.0f,  0.0f,  0.0f, 1.0f,
            25.0,  -0.5, -25.0,  25.0f, 0.0f,  0.0f,  0.0f, 1.0f,
            -25.0, -0.5, -25.0,  0.0f, 0.0f, 0.0f,  0.0f, 1.0f,


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


    float value = 25;
    // bind vao,vbo
    float verticesOutBox[] = {
            // position,texcoords,normal
            -value, -value, -value,  0.0f, 0.0f, 0.0f,  0.0f, -1.0f,
            value, -value, -value,  value, 0.0f,  0.0f,  0.0f, -1.0f,
            value,  value, -value,  value, value,  0.0f,  0.0f, -1.0f,
            value,  value, -value,  value, value,  0.0f,  0.0f, -1.0f,
            -value,  value, -value,  0.0f, value,  0.0f,  0.0f, -1.0f,
            -value, -value, -value,  0.0f, 0.0f,  0.0f,  0.0f, -1.0f,

            -value, -value,  value,  0.0f, 0.0f,   0.0f,  0.0f,  1.0f,
            value, -value,  value,  value, 0.0f,   0.0f,  0.0f,  1.0f,
            value,  value,  value,  value, value,   0.0f,  0.0f,  1.0f,
            value,  value,  value,  value, value,   0.0f,  0.0f,  1.0f,
            -value,  value,  value,  0.0f, value,   0.0f,  0.0f,  1.0f,
            -value, -value,  value,  0.0f, 0.0f,   0.0f,  0.0f,  1.0f,

            -value,  value,  value,  value, 0.0f,  1.0f,  0.0f,  0.0f,
            -value,  value, -value,  value, value,  1.0f,  0.0f,  0.0f,
            -value, -value, -value,  0.0f, value,  1.0f,  0.0f,  0.0f,
            -value, -value, -value,  0.0f, value,  1.0f,  0.0f,  0.0f,
            -value, -value,  value,  0.0f, 0.0f,  1.0f,  0.0f,  0.0f,
            -value,  value,  value,  value, 0.0f,  1.0f,  0.0f,  0.0f,

            value,  value,  value,  value, 0.0f,  1.0f,  0.0f,  0.0f,
            value,  value, -value,  value, value,  1.0f,  0.0f,  0.0f,
            value, -value, -value,  0.0f, value,  1.0f,  0.0f,  0.0f,
            value, -value, -value,  0.0f, value,  1.0f,  0.0f,  0.0f,
            value, -value,  value,  0.0f, 0.0f,  1.0f,  0.0f,  0.0f,
            value,  value,  value,  value, 0.0f,  1.0f,  0.0f,  0.0f,

            -value, -value, -value,  0.0f, value,  0.0f, -1.0f,  0.0f,
            value, -value, -value,  value, value,  0.0f, -1.0f,  0.0f,
            value, -value,  value,  value, 0.0f,  0.0f, -1.0f,  0.0f,
            value, -value,  value,  value, 0.0f,  0.0f, -1.0f,  0.0f,
            -value, -value,  value,  0.0f, 0.0f,  0.0f, -1.0f,  0.0f,
            -value, -value, -value,  0.0f, value,  0.0f, -1.0f,  0.0f,

            -value,  value, -value,  0.0f, value,   0.0f,  1.0f,  0.0f,
            value,  value, -value,  value, value,   0.0f,  1.0f,  0.0f,
            value,  value,  value,  value, 0.0f,   0.0f,  1.0f,  0.0f,
            value,  value,  value,  value, 0.0f,   0.0f,  1.0f,  0.0f,
            -value,  value,  value,  0.0f, 0.0f,   0.0f,  1.0f,  0.0f,
            -value,  value, -value,  0.0f, value,   0.0f,  1.0f,  0.0f
    };

    glBindBuffer(GL_ARRAY_BUFFER, vboIds[3]);
    glBufferData(GL_ARRAY_BUFFER,sizeof(verticesOutBox),verticesOutBox,GL_STATIC_DRAW);

    glBindVertexArray(vaoIds[3]);
    glBindBuffer(GL_ARRAY_BUFFER, vboIds[3]);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0,3,GL_FLOAT,GL_FALSE,sizeof(GLfloat) * 8,(const void *)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1,2,GL_FLOAT,GL_FALSE,sizeof(GLfloat) * 8,(const void *)(sizeof(GLfloat) * 3));
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2,3,GL_FLOAT,GL_FALSE,sizeof(GLfloat) * 8,(const void *)(sizeof(GLfloat) * 5));
    glBindVertexArray(0);
}

void ShadowCubeSample::generateFbo() {

    // 深度纹理设置有问题，提示不完整，用颜色纹理来代替
    GLenum none = GL_ONE;
    glGenTextures(1, &depthCubeTexture);
    glBindTexture(GL_TEXTURE_CUBE_MAP, depthCubeTexture);
    // GL_UNSIGNED_SHORT 这个是设置的重点
    for (int i = 0; i < 6; ++i) {
        glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_DEPTH_COMPONENT,
                     SHADOW_WIDTH, SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_UNSIGNED_SHORT, NULL);
    }

    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    glBindTexture(GL_TEXTURE_CUBE_MAP, 0);

    glGenFramebuffers(1,&m_fboId);
    glBindFramebuffer(GL_FRAMEBUFFER,m_fboId);
    glDrawBuffers(1,&none);
    glBindTexture(GL_TEXTURE_CUBE_MAP, depthCubeTexture);
    glFramebufferTexture2D(GL_FRAMEBUFFER,GL_DEPTH_ATTACHMENT,GL_TEXTURE_CUBE_MAP,depthCubeTexture,0);
//    glActiveTexture(GL_TEXTURE0);

    GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
    if (status != GL_FRAMEBUFFER_COMPLETE){
        LOGCATE("ShadowCubeSample glCheckFramebufferStatus != GL_FRAMEBUFFER_COMPLETE  error:0x%x",status);
    }
    glCheckError("depth complete");
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}


void ShadowCubeSample::draw() {

    if (!mShader->ID) return;

    glEnable(GL_DEPTH_TEST);

//    glBindFramebuffer(GL_FRAMEBUFFER,m_fboId);
//    glViewport(0,0,SHADOW_WIDTH,SHADOW_HEIGHT);
//    glClear( GL_DEPTH_BUFFER_BIT);
//    glColorMask ( GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE );
//    glEnable ( GL_POLYGON_OFFSET_FILL );
//    glClearColor(1.0, 1.0, 1.0, 1.0);
//    glCullFace(GL_FRONT);
//    renderSimpleFbo(mShadowShader);
//    glCullFace(GL_BACK);
//    glDisable( GL_POLYGON_OFFSET_FILL );
//    glBindFramebuffer(GL_FRAMEBUFFER,0);
//    glColorMask ( GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE );

    glViewport(0,0,screenWidth,screenHeight);
    glClear ( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
    glClearColor(1.0, 1.0, 1.0, 1.0);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, m_TextureId);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, mTextureIds[0]);
    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, depthCubeTexture);
    renderScene(mShader);

}

/**
 * 渲染场景
 */
void ShadowCubeSample::renderScene(Shader *pShader) {
    // Use the program object
    UpdateMvp();
    // View matrix
    glm::vec3 viewPos = glm::vec3(-1.0, 2.0, 3.0);
    glm::mat4 View = glm::lookAt(
            viewPos, // Camera is at (0,0,1), in World Space
            glm::vec3(0.0, 0.0, 0.0), // and looks at the origin
            glm::vec3(0.0, 1.0, 0.0)  // Head is up (set to 0,-1,0 to look upside-down)
    );
    glm::mat4 Model = glm::mat4(1.0f);
    glm::mat4 Projection = glm::perspective(45.0f, (float )screenWidth/(float )screenHeight, 0.1f, 100.f);
    glm::mat4 mMvp;
    // 画灯
    glm::vec3 lightColor = glm::vec3 (1.0);
    glm::vec3 lightPos = glm::vec3(0.0f,  3.0f, 0.0f);
    glm::vec3 ambientColor = glm::vec3 (0.5);

    mMvp = Projection * View * Model;

/***------阴影矩阵---------***/

    GLfloat near_plane = 1.0f, far_plane = 7.5f;
    glm::mat4 lightProjection = glm::ortho(-10.0f, 10.0f, -10.0f, 10.0f, near_plane, far_plane);
    glm::mat4 lightView = glm::lookAt(glm::vec3 (-2.0f,4.0f,-1.0f), glm::vec3(0.0f, 0.0f, 0.0f),
                                      glm::vec3(0.0f, 1.0f, 0.0f));

/***------阴影矩阵---------***/

    // 画箱子

    pShader->use();
    pShader->setVec3("viewPos",viewPos);
    pShader->setInt("type", 1);
    pShader->setFloat("material.shininess", 8.0);
    pShader->setVec3("pointLight.lightColor", lightColor);
    pShader->setVec3("pointLight.position", lightPos);
    pShader->setVec3("pointLight.ambient", ambientColor);
    pShader->setVec3("pointLight.diffuse", 1.0f, 1.0f, 1.0f);
    pShader->setVec3("pointLight.specular", 1.0f, 1.0f, 1.0f);
    pShader->setFloat("pointLight.constant", 1.0f);
    pShader->setFloat("pointLight.linear", 0.09);
    pShader->setFloat("pointLight.quadratic", 0.032);
    pShader->setMat4("lightSpaceMatrix",lightProjection * lightView);

    for (int i = 0; i < models->length(); ++i) {
        glm::mat4 currentModel = glm::translate(mBaseModel,models[i]);
        currentModel = glm::scale(currentModel,glm::vec3(0.3f,0.3f,0.3f));
        if (i == 2) {
            currentModel = glm::rotate(currentModel,10.0f,glm::vec3(1.0,1.0,0.0));
            currentModel = glm::scale(currentModel,glm::vec3(0.5f,0.5f,0.5f));
        }
        mMvp = Projection * View * currentModel;
        pShader->setMat4("mvp", mMvp);
        pShader->setMat4("model", currentModel);
        pShader->setInt("material.ambient", 0);
        pShader->setInt("material.diffuse", 0);
        pShader->setInt("material.specular", 0);
        pShader->setInt("shadowMap", 2);
        glBindVertexArray(vaoIds[0]);
        pShader->setInt("type", 0);
        glDrawArrays(GL_TRIANGLES,0,36);
        glBindVertexArray(0);
    }


    glm::mat4 currentModel = mBaseModel;
    mMvp = Projection * View * currentModel;
    pShader->setInt("material.ambient", 1);
    pShader->setInt("material.diffuse", 1);
    pShader->setInt("material.specular", 1);
    drawOutBox(pShader, mMvp);


}


void ShadowCubeSample::Destroy() {
    glDeleteVertexArrays(4,vaoIds);
    glDeleteBuffers(4,vboIds);
    glDeleteFramebuffers(1,&m_fboId);
    glDeleteTextures(2,mTextureIds);
    glDeleteTextures(1,&m_TextureId);
    if (mLightShader){
        mLightShader->Destroy();
        delete mLightShader;
        mLightShader = 0;
        mShadowShader->Destroy();
        delete mShadowShader;
        mShadowShader = 0;
    }
}

void ShadowCubeSample::renderSimpleFbo(Shader *pShader) {
// Use the program object


// 以光的视角去渲染图形
    GLfloat near_plane = 1.0f, far_plane = 7.5f;
    glm::mat4 lightProjection = glm::ortho(-10.0f, 10.0f, -10.0f, 10.0f, near_plane, far_plane);
    glm::mat4 lightView = glm::lookAt(glm::vec3 (-2.0f,4.0f,-1.0f), glm::vec3(0.0f, 0.0f, 0.0f),
                                      glm::vec3(0.0f, 1.0f, 0.0f));
    // 画箱子

    pShader->use();

    for (int i = 0; i < models->length(); ++i) {
        glm::mat4 Model = glm::mat4(1.0f);
        glm::mat4 currentModel = glm::translate(Model,models[i]);
        currentModel = glm::scale(currentModel,glm::vec3(0.5f,0.5,0.5f));
        if (i == 2) {
            currentModel = glm::rotate(currentModel,10.0f,glm::vec3(1.0,1.0,0.0));
            currentModel = glm::scale(currentModel,glm::vec3(0.5f,0.5f,0.5f));
        }
        glm::mat4 mMvp = lightProjection * lightView * currentModel;
        pShader->setMat4("mvp", mMvp);
        glBindVertexArray(vaoIds[0]);
        glDrawArrays(GL_TRIANGLES,0,36);
        glBindVertexArray(0);
    }
}

void ShadowCubeSample::drawOutBox(Shader* pshader, glm::mat4 mvp) {
    pshader->setMat4("mvp", mvp);
    glBindVertexArray(vaoIds[3]);
    glDrawArrays(GL_TRIANGLES,0,36);
    glBindVertexArray(0);
}





//
// Created by Admin on 2020/8/25.
//


#include "LightSample.h"
#include <GLES3/gl3.h>
#include <camera.h>
#include <OpenGLImageDef.h>
#include "stb_image.h"
#include "CustomGLUtils.h"
#include "ImageDef.h"

void LightSample::init(const char * vShaderStr,const char * fShaderStr) {

    mShader = new Shader(vShaderStr,fShaderStr);
    lightShader = new Shader(vShaderStr, readGLSLStrFromFile("light/lightfragment.glsl").c_str());
    float verticesBig[] = {
                // positions          // normals           // texture coords
                -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f,  0.0f,
                        0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f,  0.0f,
                        0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f,  1.0f,
                        0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f,  1.0f,
                        -0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f,  1.0f,
                        -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f,  0.0f,

                        -0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f,  0.0f,
                        0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f,  0.0f,
                        0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f,  1.0f,
                        0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f,  1.0f,
                        -0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f,  1.0f,
                        -0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f,  0.0f,

                        -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f,  0.0f,
                        -0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  1.0f,  1.0f,
                        -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
                        -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
                        -0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  0.0f,  0.0f,
                        -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f,  0.0f,

                        0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f,  0.0f,
                        0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  1.0f,  1.0f,
                        0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
                        0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
                        0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  0.0f,  0.0f,
                        0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f,  0.0f,

                        -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f,  1.0f,
                        0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  1.0f,  1.0f,
                        0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f,  0.0f,
                        0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f,  0.0f,
                        -0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  0.0f,  0.0f,
                        -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f,  1.0f,

                        -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f,  1.0f,
                        0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  1.0f,  1.0f,
                        0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f,  0.0f,
                        0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f,  0.0f,
                        -0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  0.0f,  0.0f,
                        -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f,  1.0f
            };
    glGenBuffers(1,&vboIds[0]);
    glBindBuffer(GL_ARRAY_BUFFER,vboIds[0]);
    glBufferData(GL_ARRAY_BUFFER,sizeof(verticesBig),verticesBig,GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER,0);

    glGenVertexArrays(1,&lightVao);
    glBindVertexArray(lightVao);
    glBindBuffer(GL_ARRAY_BUFFER,vboIds[0]);
    // Load the vertex position
    glVertexAttribPointer (0, 3, GL_FLOAT,
                           GL_FALSE, 8 * sizeof(GL_FLOAT), (const void *)0);
    glEnableVertexAttribArray (0);
    glVertexAttribPointer (1, 3, GL_FLOAT,
                           GL_FALSE, 8 * sizeof(GL_FLOAT), (const void *)(3* sizeof(GLfloat)));
    glEnableVertexAttribArray (1);
    glVertexAttribPointer (2, 2, GL_FLOAT,
                           GL_FALSE, 8 * sizeof(GL_FLOAT), (const void *)(6* sizeof(GLfloat)));
    glEnableVertexAttribArray (2);
    glBindVertexArray(0);
    LOGCATE("upload cost:%lld",(GetSysCurrentTime() - startTime));

    const char *fileName = "/storage/emulated/0/ffmpegtest/filterImg/test_box_border.png";
    LoadImageInfo imageInfo;
    imageData = stbi_load(fileName,&imageInfo.width,&imageInfo.height,&imageInfo.channels,0);
    logLoadImageInfo(imageInfo);

    const char *fileName2 = "/storage/emulated/0/ffmpegtest/filterImg/test_box_border_rgb.png";
    LoadImageInfo imageInfo2;
    stbi_uc *imageData2 = stbi_load(fileName2, &imageInfo2.width, &imageInfo2.height,
                                    &imageInfo2.channels, 0);
    logLoadImageInfo(imageInfo2);

    const char *fileName3 = "/storage/emulated/0/ffmpegtest/filterImg/test_box_green.jpg";
    LoadImageInfo imageInfo3;
    stbi_uc *imageData3 = stbi_load(fileName3, &imageInfo3.width, &imageInfo3.height,
                                    &imageInfo3.channels, 0);
    logLoadImageInfo(imageInfo3);

    const char *fileName4 = "/storage/emulated/0/ffmpegtest/filterImg/test_box_robot.jpg";
    LoadImageInfo imageInfo4;
    stbi_uc *imageData4 = stbi_load(fileName4, &imageInfo4.width, &imageInfo4.height,
                                    &imageInfo4.channels, 0);
    logLoadImageInfo(imageInfo4);


    glGenTextures(5,mLightTexture);
    glBindTexture(GL_TEXTURE_2D,mLightTexture[0]);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexImage2D(GL_TEXTURE_2D,0,GL_RGBA,m_RenderImage.width,m_RenderImage.height,0,GL_RGBA,GL_UNSIGNED_BYTE,m_RenderImage.ppPlane[0]);

    glBindTexture(GL_TEXTURE_2D,mLightTexture[1]);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexImage2D(GL_TEXTURE_2D,0,GL_RGBA,imageInfo.width,imageInfo.height,0,GL_RGBA,GL_UNSIGNED_BYTE,imageData);

    glBindTexture(GL_TEXTURE_2D,mLightTexture[2]);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexImage2D(GL_TEXTURE_2D,0,GL_RGBA,imageInfo2.width,imageInfo2.height,0,GL_RGBA,GL_UNSIGNED_BYTE,imageData2);

    glBindTexture(GL_TEXTURE_2D,mLightTexture[3]);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexImage2D(GL_TEXTURE_2D,0,GL_RGBA,imageInfo3.width,imageInfo3.height,0,GL_RGBA,GL_UNSIGNED_BYTE,imageData3);

    glBindTexture(GL_TEXTURE_2D,mLightTexture[4]);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexImage2D(GL_TEXTURE_2D,0,GL_RGBA,imageInfo4.width,imageInfo4.height,0,GL_RGBA,GL_UNSIGNED_BYTE,imageData4);


    glBindTexture(GL_TEXTURE_2D,0);
    stbi_image_free(imageData2);
    stbi_image_free(imageData3);
    stbi_image_free(imageData4);
}


void LightSample::draw() {

    if(mShader->ID == GL_NONE) return;
    glClear(GL_STENCIL_BUFFER_BIT | GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    for (int i = 0; i < 4; ++i) {
        drawWhole(i);
    }
}

void LightSample::drawWhole(int index) {

    glm::vec3 bgs[4] = {
            glm::vec3 (0.74,0.52f,0.29f),
            glm::vec3 (0.1f,0.1f,0.1f),
            glm::vec3 (1.0f,0.0f,0.0f),
            glm::vec3 (1.0f,1.0f,1.0f),
    };
    glm::vec4 sizes[4] = {
            // 0.0,0.0 位于左下角
            glm::vec4 (0.0,0.0,screenWidth/2,screenHeight/2),
            glm::vec4 (0.0,screenHeight/2,screenWidth/2,screenHeight/2),
            glm::vec4 (screenWidth/2, screenHeight/2, screenWidth/2, screenHeight/2),
            glm::vec4 (screenWidth/2,0.0f,screenWidth/2,screenHeight/2),
    };
    glm::vec3 result = bgs[3];
    glm::vec4 sizeResult = sizes[index];

    glViewport(sizeResult.x,sizeResult.y,sizeResult.z,sizeResult.w);
//    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
    glClearColor(result.r,result.g,result.b,1.0);


    if (second == 0L){
        second = GetSysCurrentTime();
    }
    float diffValue = (float)(GetSysCurrentTime() - second) / (float)1000;
//    LOGCATE("log second:%f",diffValue);

    UpdateMvp();

    float radius = 8.0;
    float camX = sin(diffValue) * radius;
    glm::vec3 cameraPos = glm::vec3(0.0f,0.0f,3.0f);
    Camera camera(cameraPos);
    glm::vec3 lightPos = camera.Position;

    mShader->use();
    glEnable(GL_DEPTH_TEST);
    glBindVertexArray(lightVao);

    // positions of the point lights
    glm::vec3 pointLightPositions[] = {
            glm::vec3( 0.7f,  0.2f,  2.0f),
            glm::vec3( 2.3f, -3.3f, -4.0f),
            glm::vec3(-4.0f,  2.0f, -12.0f),
            glm::vec3( 0.0f,  0.0f, -3.0f)
    };

    mShader->setVec3("dirLight.direction", -0.2f, -1.0f, -0.3f);
    mShader->setVec3("dirLight.ambient", 0.05f, 0.05f, 0.05f);
    mShader->setVec3("dirLight.diffuse", 0.4f, 0.4f, 0.4f);
    mShader->setVec3("dirLight.specular", 0.5f, 0.5f, 0.5f);

    // point light 1
    mShader->setVec3("pointLights[0].position", pointLightPositions[0]);
    mShader->setVec3("pointLights[0].ambient", 0.05f, 0.05f, 0.05f);
    mShader->setVec3("pointLights[0].diffuse", 0.8f, 0.8f, 0.8f);
    mShader->setVec3("pointLights[0].specular", 1.0f, 1.0f, 1.0f);
    mShader->setFloat("pointLights[0].constant", 1.0f);
    mShader->setFloat("pointLights[0].linear", 0.09);
    mShader->setFloat("pointLights[0].quadratic", 0.032);
    // point light 2
    mShader->setVec3("pointLights[1].position", pointLightPositions[1]);
    mShader->setVec3("pointLights[1].ambient", 0.05f, 0.05f, 0.05f);
    mShader->setVec3("pointLights[1].diffuse", 0.8f, 0.8f, 0.8f);
    mShader->setVec3("pointLights[1].specular", 1.0f, 1.0f, 1.0f);
    mShader->setFloat("pointLights[1].constant", 1.0f);
    mShader->setFloat("pointLights[1].linear", 0.09);
    mShader->setFloat("pointLights[1].quadratic", 0.032);
    // point light 3
    mShader->setVec3("pointLights[2].position", pointLightPositions[2]);
    mShader->setVec3("pointLights[2].ambient", 0.05f, 0.05f, 0.05f);
    mShader->setVec3("pointLights[2].diffuse", 0.8f, 0.8f, 0.8f);
    mShader->setVec3("pointLights[2].specular", 1.0f, 1.0f, 1.0f);
    mShader->setFloat("pointLights[2].constant", 1.0f);
    mShader->setFloat("pointLights[2].linear", 0.09);
    mShader->setFloat("pointLights[2].quadratic", 0.032);
    // point light 4
    mShader->setVec3("pointLights[3].position", pointLightPositions[3]);
    mShader->setVec3("pointLights[3].ambient", 0.05f, 0.05f, 0.05f);
    mShader->setVec3("pointLights[3].diffuse", 0.8f, 0.8f, 0.8f);
    mShader->setVec3("pointLights[3].specular", 1.0f, 1.0f, 1.0f);
    mShader->setFloat("pointLights[3].constant", 1.0f);
    mShader->setFloat("pointLights[3].linear", 0.09);
    mShader->setFloat("pointLights[3].quadratic", 0.032);
//
    mShader->setFloat("material.shininess", 8.0);
    mShader->setInt("material.ambient", 0);
    mShader->setInt("material.diffuse", 1);
    mShader->setInt("material.specular", 2);

    mShader->setVec3("lightPos", lightPos);
    mShader->setVec3("viewPos", camera.Position);
    mShader->setVec3("light.direction", camera.Front);

    mShader->setVec3("light.ambient", 0.3, 0.3, 0.3);
    mShader->setVec3("light.diffuse", 0.5, 0.5, 0.5);
    mShader->setVec3("light.specular", 1.0, 1.0, 1.0);
    mShader->setFloat("light.constant", 1.0f);
    mShader->setFloat("light.linear", 0.09f);
    mShader->setFloat("light.quadratic", 0.032f);
    mShader->setFloat("light.cutOff", cos(glm::radians(12.5f)));
    mShader->setFloat("light.outerCutOff", cos(glm::radians(17.5f)));
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, mLightTexture[0]);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, mLightTexture[2]);
    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, mLightTexture[3]);


    glm::vec3 cubePositions[] = {
        glm::vec3( 0.0f,  0.0f,  0.0f),
        glm::vec3( 2.0f,  5.0f, -15.0f),
        glm::vec3(-1.5f, -2.2f, -2.5f),
        glm::vec3(-3.8f, -2.0f, -12.3f),
        glm::vec3( 2.4f, -0.4f, -3.5f),
        glm::vec3(-1.7f,  3.0f, -7.5f),
        glm::vec3( 1.3f, -2.0f, -2.5f),
        glm::vec3( 1.5f,  2.0f, -2.5f),
        glm::vec3( 1.5f,  0.2f, -1.5f),
        glm::vec3(-1.3f,  1.0f, -1.5f)
};

    for(unsigned int i = 0; i < 10; i++)
    {
        glm::mat4 resultModel;
        resultModel = glm::translate(mBaseModel, cubePositions[i]);
        float angle = 20.0f * i;
        resultModel = glm::rotate(resultModel, glm::radians(angle), glm::vec3(1.0f, 0.3f, 0.5f));
        mShader->setMat4("model", resultModel);
        mShader->setMat4("mMvpMatrix", mBaseProjection * mBaseView * resultModel);
        glDrawArrays(GL_TRIANGLES, 0, 36);
    }


    lightShader->use();
    glEnable(GL_DEPTH_TEST);
    glBindVertexArray(lightVao);
    glm::vec3 colors[4] = {
            glm::vec3 (1.0f,0.0f,0.0f),
            glm::vec3 (0.0f,1.0f,0.0f),
            glm::vec3 (0.0f,0.0f,1.0f),
            glm::vec3 (1.0f,1.0f,1.0f)
    };
    for (int i = 0; i < 4; ++i) {
        glm::mat4 model = glm::mat4();
        glm::mat4 projection = glm::mat4(1.0f);
        model = glm::translate(model, pointLightPositions[i]);
        model = glm::scale(model,  glm::vec3(0.2f));
        projection = glm::perspective(glm::radians(45.0f),1.33f,0.1f,100.0f);
        glm::mat4 mvp = projection * mBaseView * model;
        lightShader->setMat4("model", model);
        lightShader->setMat4("mMvpMatrix", mvp);
        lightShader->setVec3("temparyLightColor", colors[index]);
        glDrawArrays(GL_TRIANGLES, 0, 36);
    }
}

void LightSample::UpdateTransformMatrix(float rotateX, float rotateY, float scaleX, float scaleY)
{

//    GLSampleBase::UpdateTransformMatrix(rotateX, rotateY, scaleX, scaleY);
    m_AngleX = static_cast<int>(rotateX);
    m_AngleY = static_cast<int>(rotateY);
    m_ScaleX = scaleX;
    m_ScaleY = scaleY;

//    LOGCATE("BasicLightingSample::UpdateMVPMatrix angleX = %d, angleY = %d, ratio = %f", angleX,
//            angleY, ratio);
}

void LightSample::Destroy()
{
    if (m_ProgramObj)
    {
        LOGCATE("before over %p",imageData);
        if (imageData){
            stbi_image_free(imageData);
            LOGCATE("after over %p",imageData);
            imageData = nullptr;
        }
        if (lightShader){
            lightShader->Destroy();
            delete lightShader;
        }
        glDeleteTextures(10,mLightTexture);
        glDeleteProgram(m_ProgramObj);
        glDeleteVertexArrays(1,&lightVao);
        glDeleteBuffers(1,&vboIds[0]);
    }

}

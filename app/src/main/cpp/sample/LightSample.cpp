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

    m_ProgramObj = CreateProgram(vShaderStr, fShaderStr, m_VertexShader, m_FragmentShader);
    if (m_ProgramObj)
    {
    }
    else
    {
        LOGCATE("TextureMapSample::Init create program fail");
    }
    const char * fragmentStr =
            "#version 300 es\n"
            "precision mediump float;                             \n"
            "out vec4 FragColor;              \n"
            "void main()                                         \n"
            "{                                                   \n"
            "  FragColor = vec4(1.0f,1.0f,1.0f,1.0f);     \n"
            "}";
    lightProgram = CreateProgram(vShaderStr, fragmentStr, m_VertexShader, m_FragmentShader);
    if (!lightProgram){
        LOGCATE("TextureMapSample::Init lightProgram fail");
    }
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
    printLoadImageInfo(imageInfo);

    const char *fileName2 = "/storage/emulated/0/ffmpegtest/filterImg/test_box_border_rgb.png";
    LoadImageInfo imageInfo2;
    stbi_uc *imageData2 = stbi_load(fileName2, &imageInfo2.width, &imageInfo2.height,
                                    &imageInfo2.channels, 0);
    printLoadImageInfo(imageInfo2);

    const char *fileName3 = "/storage/emulated/0/ffmpegtest/filterImg/test_box_green.jpg";
    LoadImageInfo imageInfo3;
    stbi_uc *imageData3 = stbi_load(fileName3, &imageInfo3.width, &imageInfo3.height,
                                    &imageInfo3.channels, 0);
    printLoadImageInfo(imageInfo3);

    const char *fileName4 = "/storage/emulated/0/ffmpegtest/filterImg/test_box_robot.jpg";
    LoadImageInfo imageInfo4;
    stbi_uc *imageData4 = stbi_load(fileName4, &imageInfo4.width, &imageInfo4.height,
                                    &imageInfo4.channels, 0);
    printLoadImageInfo(imageInfo4);


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

    if(m_ProgramObj == GL_NONE) return;

    glClear(GL_STENCIL_BUFFER_BIT | GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glClearColor(1.0, 1.0, 1.0, 1.0);

    if (second == 0L){
        second = GetSysCurrentTime();
    }
    float diffValue = (float)(GetSysCurrentTime() - second) / (float)1000;
//    LOGCATE("log second:%f",diffValue);
    glm::vec3 lightPos = glm::vec3( 0.0,  1.0f,  2.0f);
    float radius = 8.0;
    float camX = sin(diffValue) * radius;
    glm::vec3 cameraPos = glm::vec3(-3,0.0f,3.0);
    Camera camera(cameraPos);

    glUseProgram (m_ProgramObj);
    glEnable(GL_DEPTH_TEST);
    glBindVertexArray(lightVao);
    setVec3(m_ProgramObj,"lightPos",lightPos);
    setVec3(m_ProgramObj,"viewPos",camera.Position);
    UpdateMvp( m_AngleX, m_AngleY, (float) screenWidth / (float) screenHeight);
    setMat4(m_ProgramObj, "model", model);
    setMat4(m_ProgramObj, "mMvpMatrix", mMvpMatrix);
    setFloat(m_ProgramObj,"material.shininess",8.0);
    setInt(m_ProgramObj,"textSample",0);
    setInt(m_ProgramObj,"borderSample",1);
    setInt(m_ProgramObj,"flowSample",2);
    setVec3(m_ProgramObj,"light.ambient",0.3,0.3,0.3);
    setVec3(m_ProgramObj,"light.diffuse",0.5,0.5,0.5);
    setVec3(m_ProgramObj,"light.specular",1.0,1.0,1.0);
    setVec3(m_ProgramObj,"light.direction",camera.Front);
    setFloat(m_ProgramObj,"light.cutOff",   glm::cos(glm::radians(12.5f)));
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D,mLightTexture[0]);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D,mLightTexture[2]);
    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D,mLightTexture[3]);
    glDrawArrays(GL_TRIANGLES, 0, 36);


    glUseProgram (lightProgram);
    glEnable(GL_DEPTH_TEST);
    glBindVertexArray(lightVao);
    model = glm::mat4();
    projection = glm::mat4(1.0f);
    model = glm::translate(model, lightPos);
    model = glm::scale(model,  glm::vec3(0.2f));
    projection = glm::perspective(glm::radians(45.0f),1.33f,0.1f,100.0f);
    setMat4(lightProgram, "model", model);
    setMat4(lightProgram, "view", view);
    setMat4(lightProgram, "projection", projection);
    glDrawArrays(GL_TRIANGLES, 0, 36);
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

void LightSample::UpdateMultipleMvp(int angleX, int angleY, float ratio) {
    angleX = angleX % 360;
    angleY = angleY % 360;

    //转化为弧度角
    float radiansX = static_cast<float>(MATH_PI / 180.0f * angleX);
    float radiansY = static_cast<float>(MATH_PI / 180.0f * angleY);


    // Projection matrix
//glm::mat4 Projection = glm::ortho(-ratio, ratio, -1.0f, 1.0f, 0.0f, 100.0f);
//glm::mat4 Projection = glm::frustum(-ratio, ratio, -1.0f, 1.0f, 4.0f, 100.0f);
    glm::mat4 Projection = glm::perspective(45.0f, ratio, 0.1f, 100.f);

    // View matrix
    glm::mat4 View = glm::lookAt(
            glm::vec3(-3, 0, 3), // Camera is at (0,0,1), in World Space
            glm::vec3(0, 0, 0), // and looks at the origin
            glm::vec3(0, 1, 0)  // Head is up (set to 0,-1,0 to look upside-down)
    );

    // Model matrix
    glm::mat4 Model = glm::mat4(1.0f);
    Model = glm::scale(Model, glm::vec3(m_ScaleX, m_ScaleX, m_ScaleX));
    Model = glm::rotate(Model, radiansX, glm::vec3(1.0f, 0.0f, 0.0f));
    Model = glm::rotate(Model, radiansY, glm::vec3(0.0f, 1.0f, 0.0f));
    Model = glm::translate(Model, glm::vec3(0.0f, 0.0f, 0.0f));

    model = Model;
    mMvpMatrix = Projection * View * Model;
}


void LightSample::UpdateMvp(int angleX, int angleY, float ratio) {
    angleX = angleX % 360;
    angleY = angleY % 360;

    //转化为弧度角
    float radiansX = static_cast<float>(MATH_PI / 180.0f * angleX);
    float radiansY = static_cast<float>(MATH_PI / 180.0f * angleY);


    // Projection matrix
//glm::mat4 Projection = glm::ortho(-ratio, ratio, -1.0f, 1.0f, 0.0f, 100.0f);
//glm::mat4 Projection = glm::frustum(-ratio, ratio, -1.0f, 1.0f, 4.0f, 100.0f);
    glm::mat4 Projection = glm::perspective(45.0f, ratio, 0.1f, 100.f);

    // View matrix
    glm::mat4 View = glm::lookAt(
            glm::vec3(-3, 0, 3), // Camera is at (0,0,1), in World Space
            glm::vec3(0, 0, 0), // and looks at the origin
            glm::vec3(0, 1, 0)  // Head is up (set to 0,-1,0 to look upside-down)
    );

    // Model matrix
    glm::mat4 Model = glm::mat4(1.0f);
    Model = glm::scale(Model, glm::vec3(m_ScaleX, m_ScaleX, m_ScaleX));
    Model = glm::rotate(Model, radiansX, glm::vec3(1.0f, 0.0f, 0.0f));
    Model = glm::rotate(Model, radiansY, glm::vec3(0.0f, 1.0f, 0.0f));
    Model = glm::translate(Model, glm::vec3(0.0f, 0.0f, 0.0f));

    model = Model;

    mMvpMatrix = Projection * View * Model;
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
        glDeleteTextures(10,mLightTexture);
        glDeleteProgram(m_ProgramObj);
        glDeleteVertexArrays(1,&lightVao);
        glDeleteBuffers(1,&vboIds[0]);
    }

}

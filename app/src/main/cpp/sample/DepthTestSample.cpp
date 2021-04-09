//
// Created by Admin on 2020/8/25.
//


#include "DepthTestSample.h"
#include <GLES3/gl3.h>
#include "stb_image.h"
#include "CustomGLUtils.h"
#include "ImageDef.h"

void DepthTestSample::init(const char * vShaderStr,const char * fShaderStr) {

    m_ProgramObj = CreateProgram(vShaderStr, fShaderStr, m_VertexShader, m_FragmentShader);
    if (m_ProgramObj)
    {
        m_SamplerLoc = glGetUniformLocation(m_ProgramObj, "s_TextureMap");
    }
    else
    {
        LOGCATE("TextureMapSample::Init create program fail");
    }
    const char * depthFragStr =
            "#version 300 es\n"
            "precision mediump float;\n"
            "layout(location = 0) out vec4 outColor;\n"
            "uniform sampler2D s_TextureMap;\n"
            "\n"
            "void main()                                         \n"
            "{\n"
            "  outColor = vec4(1.0, 0.0, 0.0, 1.0);\n"
            "}";
    mDepthProgram = CreateProgram(vShaderStr,depthFragStr, m_VertexShader, m_FragmentShader);
    if (!mDepthProgram){
        LOGCATE("init depth program failed");
    }
    delete vShaderStr;
    delete fShaderStr;

    int64_t startTime= GetSysCurrentTime();
    // test1,2,3 ...
    const char * filePath = "/storage/emulated/0/ffmpegtest/filterImg/marble.jpg";
//    const char * filePath2 = "/storage/emulated/0/ffmpegtest/filterImg/test2.png";
    LoadImageInfo imageInfo;
    imageInfo.loadImage(filePath);
    //    LOGCATE("image width:%d height:%d channel:%d data:%p costTime:%lld",localWidth,localHeight,nrChannels,imageData,
//            (GetSysCurrentTime() - startTime));
    startTime = GetSysCurrentTime();
    //create RGBA texture
    glGenTextures(1, &m_TextureId);
    glBindTexture(GL_TEXTURE_2D, m_TextureId);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    imageInfo.uploadImageTex2D();
    //    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, m_RenderImage.width, m_RenderImage.height, 0, GL_RGBA, GL_UNSIGNED_BYTE,m_RenderImage.ppPlane[0]);
    glBindTexture(GL_TEXTURE_2D, GL_NONE);
    float verticesBig[] = {
            -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
            0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
            0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
            0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
            -0.5f,  0.5f,  0.5f,  0.0f, 1.0f,
            -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,

            -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,
            0.5f, -0.5f, -0.5f,  1.0f, 0.0f,
            0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
            0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
            -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
            -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,

            -0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
            -0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
            -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
            -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
            -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
            -0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

            0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
            0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
            0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
            0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
            0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
            0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

            -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
            0.5f, -0.5f, -0.5f,  1.0f, 1.0f,
            0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
            0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
            -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
            -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,

            -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
            0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
            0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
            0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
            -0.5f,  0.5f,  0.5f,  0.0f, 0.0f,
            -0.5f,  0.5f, -0.5f,  0.0f, 1.0f
    };
    glGenBuffers(1,&vboIds[0]);
    glBindBuffer(GL_ARRAY_BUFFER,vboIds[0]);
    glBufferData(GL_ARRAY_BUFFER,sizeof(verticesBig),verticesBig,GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER,0);
    LOGCATE("upload cost:%lld",(GetSysCurrentTime() - startTime));
}

void DepthTestSample::draw() {

    if(m_ProgramObj == GL_NONE || m_TextureId == GL_NONE) return;

    glStencilOp(GL_KEEP,GL_KEEP,GL_REPLACE);
    glClear(GL_STENCIL_BUFFER_BIT | GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glClearColor(1.0, 1.0, 1.0, 1.0);




//    GLushort indices[] = { 0, 1, 2, 0, 2, 3 };
    
//    LOGCATE("imagewidth:%i   ---imageHeight:%i",m_RenderImage.width,m_RenderImage.height);
    // Use the program object
    glUseProgram (m_ProgramObj);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_STENCIL_TEST);
//
//    // 画原图像
    glStencilFunc(GL_ALWAYS,1,0xff);
    glStencilMask(0xff);
    drawOriginal();
    createMvp(m_ProgramObj, false);

    // 画边框
    glStencilFunc(GL_NOTEQUAL,1,0xff);
    glStencilMask(0x00);
    glDisable(GL_DEPTH_TEST);
    drawScale();
    createMvp(mDepthProgram,true);
//    glStencilMask(0xff);
//    glEnable(GL_DEPTH_TEST);



}

void DepthTestSample::drawOriginal() {
    glBindBuffer(GL_ARRAY_BUFFER, vboIds[0]);
    // Load the vertex position
    glVertexAttribPointer (0, 3, GL_FLOAT,
                           GL_FALSE, 5 * sizeof(GL_FLOAT), (const void *)0);
    // Load the texture coordinate
    glVertexAttribPointer (1, 2, GL_FLOAT,
                           GL_FALSE, 5 * sizeof(GL_FLOAT), (const void *)(3 * sizeof(GL_FLOAT)));

    glEnableVertexAttribArray (0);
    glEnableVertexAttribArray (1);

    // Bind the RGBA map
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, m_TextureId);

    // Set the RGBA map sampler to texture unit to 0
    glUniform1i(m_SamplerLoc, 0);

}


void DepthTestSample::Destroy()
{
    if (m_ProgramObj)
    {
        LOGCATE("before over %p",imageData);
        if (imageData){
            stbi_image_free(imageData);
            LOGCATE("after over %p",imageData);
            imageData = nullptr;
        }
        glDeleteProgram(m_ProgramObj);
        glDeleteProgram(mDepthProgram);
        glDeleteTextures(1, &m_TextureId);
        glDeleteBuffers(1,&vboIds[0]);
    }

}

void DepthTestSample::createMvp(GLint program, bool isScale) {
    model = glm::mat4(1.0f);
    view = glm::mat4(1.0f);
    projection = glm::mat4(1.0f);
    float second = 1.0f;
    if (startTime == 0L){
        startTime = GetSysCurrentTime();
    } else {
        second = (float)(GetSysCurrentTime() - startTime) / 1000;
    }

    glm::vec3 cubePositions[] = {
            glm::vec3( 0.0f,  0.0f,  0.0f),
            glm::vec3( 2.0f,  0.0f, -2.0f),
            glm::vec3(3.5f, 0.0f, -2.5f),
            glm::vec3(5.0f, 0.0f, -5.3f),
            glm::vec3( 2.4f, 0.0f, -3.5f),

            glm::vec3(-1.0f,  0.0f, -1.5f),
            glm::vec3( 1.5f, 0.0f, -2.5f),
            glm::vec3( -2.0f,  0.0f, -2.5f),
            glm::vec3( 1.5f,  0.0f, -1.5f),
            glm::vec3(-3.0f,  1.0f, -1.5f)
    };

    for (int i = 0; i < 1; ++i) {
        model = glm::translate(model, cubePositions[i]);
//        if (angle == 0) angle = 20.0f;
//        angle = glm::radians(angle);
//        if ( i == 0 || i % 3 == 0){
//            angle = angle * second;
//        }
//        LOGCATE("angle will be rotate %f",angle);
//        model = glm::rotate(model, angle, glm::vec3(1.0f, 0.3f, 0.5f));

        if (isScale){
            model = glm::scale(model,glm::vec3(1.1,1.1,1.1));
        }

        // create view matrix
        float radius = 10.0f;
        float camX = sin(second) * radius;
        float camZ = cos(second) * radius;
//        view = glm::translate(view,glm::vec3(0.0f,0.0f,-3.0f));
        view = glm::lookAt(glm::vec3(0.0,0.0,mEyeZ),glm::vec3(0.0,0.0,0.0),
                           glm::vec3(0.0,1.0,0.0));
//    LOGCATE("log width:%d height:%d",screenWidth,screenHeight);
        float ration = (float)screenWidth / (float )screenHeight;
        projection = glm::perspective(glm::radians(45.0f),1.33f,0.1f,100.0f);
        setMat4(program, "model", model);
        setMat4(program, "view", view);
        setMat4(program, "projection", projection);
        glDrawArrays(GL_TRIANGLES, 0, 6);
    }

}

void DepthTestSample::drawScale() {
    glUseProgram(mDepthProgram);
    drawOriginal();
}

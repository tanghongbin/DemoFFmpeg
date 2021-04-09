//
// Created by Admin on 2020/8/25.
//


#include "BlendSample.h"
#include <GLES3/gl3.h>
#include "stb_image.h"
#include "CustomGLUtils.h"
#include "ImageDef.h"

void BlendSample::init(const char * vShaderStr,const char * fShaderStr) {

    m_ProgramObj = CreateProgram(vShaderStr, fShaderStr, m_VertexShader, m_FragmentShader);
    if (m_ProgramObj)
    {
        m_SamplerLoc = glGetUniformLocation(m_ProgramObj, "s_TextureMap");
    }
    else
    {
        LOGCATE("TextureMapSample::Init create program fail");
    }
    delete vShaderStr;
    delete fShaderStr;

    int64_t startTime= GetSysCurrentTime();
    int localWidth;
    int localHeight;
    int nrChannels;
    // test1,2,3 ...
    const char * filePath2 = "/storage/emulated/0/ffmpegtest/filterImg/alpha_window.jpeg";
    LoadImageInfo imageInfo;
    imageInfo.loadImage(filePath2);

    startTime = GetSysCurrentTime();
    //create RGBA texture
    glGenTextures(1, &m_TextureId);
    glBindTexture(GL_TEXTURE_2D, m_TextureId);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
//    glTexImage2D(GL_TEXTURE_2D, 0, format, localWidth,localHeight, 0, format, GL_UNSIGNED_BYTE, imageData);
    imageInfo.uploadImageTex2D();
    glBindTexture(GL_TEXTURE_2D, GL_NONE);

    const char * windowPath = "/storage/emulated/0/ffmpegtest/filterImg/window.png";
    LoadImageInfo imageInfoWindow;
    imageInfoWindow.loadImage(windowPath);
    // 窗户
    glGenTextures(4, textures);
    glBindTexture(GL_TEXTURE_2D, textures[0]);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    imageInfoWindow.uploadImageTex2D();
    glBindTexture(GL_TEXTURE_2D, GL_NONE);
    float verticesBig[] = {


            //            -0.5f, -0.5f, -0.5f,  0.0f, 0.0f, // Bottom-left
//            0.5f, -0.5f, -0.5f,  1.0f, 0.0f, // bottom-right
//            0.5f,  0.5f, -0.5f,  1.0f, 1.0f, // top-right
//            0.5f,  0.5f, -0.5f,  1.0f, 1.0f, // top-right
//            -0.5f,  0.5f, -0.5f,  0.0f, 1.0f, // top-left
//            -0.5f, -0.5f, -0.5f,  0.0f, 0.0f, // bottom-left
//            // Front face
//            -0.5f, -0.5f,  0.5f,  0.0f, 0.0f, // bottom-left
//            0.5f,  0.5f,  0.5f,  1.0f, 1.0f, // top-right
//            0.5f, -0.5f,  0.5f,  1.0f, 0.0f, // bottom-right
//            0.5f,  0.5f,  0.5f,  1.0f, 1.0f, // top-right
//            -0.5f, -0.5f,  0.5f,  0.0f, 0.0f, // bottom-left
//            -0.5f,  0.5f,  0.5f,  0.0f, 1.0f, // top-left

            // back
            -0.5f, 0.5f,  -0.5f,  0.0f, 0.0f, //左上
            -0.5f,  -0.5f,  -0.5f,  0.0f, 1.0f, // 左下
            0.5f,  -0.5f,  -0.5f,  1.0f, 1.0f, // 右下
            0.5f,  -0.5f,  -0.5f,  1.0f, 1.0f, // 右下
            0.5f, 0.5f,  -0.5f,  1.0f, 0.0f, // 右上
            -0.5f, -0.5f,  -0.5f,  0.0f, 0.0f, // 左上

            // front
            -0.5f, 0.5f,  0.5f,  0.0f, 0.0f,//左上
            0.5f,  -0.5f,  0.5f,  1.0f, 1.0f,// 右下
            -0.5f,  -0.5f,  0.5f,  0.0f, 1.0f,// 左下
            0.5f,  -0.5f,  0.5f,  1.0f, 1.0f,// 右下
            -0.5f, 0.5f,  0.5f,  0.0f, 0.0f,//左上
            0.5f, 0.5f,  0.5f,  1.0f, 0.0f,// 右上


            //left
            -0.5f, 0.5f, 0.5f,  0.0f, 1.0f,
            -0.5f,  -0.5f, 0.5f,  1.0f, 1.0f,
            -0.5f,  -0.5f,  -0.5f,  1.0f, 0.0f,
            -0.5f,  -0.5f,  -0.5f,  1.0f, 0.0f,
            -0.5f, 0.5f, -0.5f,  0.0f, 1.0f,
            -0.5f, 0.5f,  0.5f,  0.0f, 0.0f,


            // right
            0.5f, 0.5f, 0.5f,  0.0f, 1.0f,
            0.5f, 0.5f, -0.5f,  0.0f, 1.0f,
            0.5f,  -0.5f, -0.5f,  1.0f, 0.0f,
            0.5f,  -0.5f, -0.5f,  1.0f, 0.0f,
            0.5f,  -0.5f,  0.5f,  1.0f, 0.0f,
            0.5f, 0.5f, 0.5f,  0.0f, 1.0f,

            // bottom
            -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
            -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
            0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
            0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
            0.5f, -0.5f, -0.5f,  1.0f, 1.0f,
            -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,

            // top
            -0.5f, 0.5f, -0.5f,  0.0f, 1.0f,
            0.5f, 0.5f,  -0.5f,  1.0f, 0.0f,
            0.5f, 0.5f,  0.5f,  1.0f, 0.0f,
            0.5f, 0.5f,  0.5f,  1.0f, 0.0f,
            -0.5f, 0.5f, 0.5f,  1.0f, 1.0f,
            -0.5f, 0.5f, -0.5f,  0.0f, 1.0f,




// Back face
//            -0.5f, -0.5f, -0.5f,  0.0f, 0.0f, // Bottom-left
//            0.5f, -0.5f, -0.5f,  1.0f, 0.0f, // bottom-right
//            0.5f,  0.5f, -0.5f,  1.0f, 1.0f, // top-right
//            0.5f,  0.5f, -0.5f,  1.0f, 1.0f, // top-right
//            -0.5f,  0.5f, -0.5f,  0.0f, 1.0f, // top-left
//            -0.5f, -0.5f, -0.5f,  0.0f, 0.0f, // bottom-left
//            // Front face
//            -0.5f, -0.5f,  0.5f,  0.0f, 0.0f, // bottom-left
//            0.5f,  0.5f,  0.5f,  1.0f, 1.0f, // top-right
//            0.5f, -0.5f,  0.5f,  1.0f, 0.0f, // bottom-right
//            0.5f,  0.5f,  0.5f,  1.0f, 1.0f, // top-right
//            -0.5f, -0.5f,  0.5f,  0.0f, 0.0f, // bottom-left
//            -0.5f,  0.5f,  0.5f,  0.0f, 1.0f, // top-left
//
//            // Left face
//            -0.5f,  0.5f,  0.5f,  1.0f, 0.0f, // top-right
//            -0.5f, -0.5f, -0.5f,  0.0f, 1.0f, // bottom-left
//            -0.5f,  0.5f, -0.5f,  1.0f, 1.0f, // top-left
//            -0.5f, -0.5f, -0.5f,  0.0f, 1.0f, // bottom-left
//            -0.5f,  0.5f,  0.5f,  1.0f, 0.0f, // top-right
//            -0.5f, -0.5f,  0.5f,  0.0f, 0.0f, // bottom-right
//            // Right face
//            0.5f,  0.5f,  0.5f,  1.0f, 0.0f, // top-left
//            0.5f,  0.5f, -0.5f,  1.0f, 1.0f, // top-right
//            0.5f, -0.5f, -0.5f,  0.0f, 1.0f, // bottom-right
//            0.5f, -0.5f, -0.5f,  0.0f, 1.0f, // bottom-right
//            0.5f, -0.5f,  0.5f,  0.0f, 0.0f, // bottom-left
//            0.5f,  0.5f,  0.5f,  1.0f, 0.0f, // top-left
//            // Bottom face
//            -0.5f, -0.5f, -0.5f,  0.0f, 1.0f, // top-right
//            0.5f, -0.5f,  0.5f,  1.0f, 0.0f, // bottom-left
//            0.5f, -0.5f, -0.5f,  1.0f, 1.0f, // top-left
//            0.5f, -0.5f,  0.5f,  1.0f, 0.0f, // bottom-left
//            -0.5f, -0.5f, -0.5f,  0.0f, 1.0f, // top-right
//            -0.5f, -0.5f,  0.5f,  0.0f, 0.0f, // bottom-right
//            // Top face
//            -0.5f,  0.5f, -0.5f,  0.0f, 1.0f, // top-left
//            0.5f,  0.5f, -0.5f,  1.0f, 1.0f, // top-right
//            0.5f,  0.5f,  0.5f,  1.0f, 0.0f, // bottom-right
//            0.5f,  0.5f,  0.5f,  1.0f, 0.0f, // bottom-right
//            -0.5f,  0.5f,  0.5f,  0.0f, 0.0f, // bottom-left
//            -0.5f,  0.5f, -0.5f,  0.0f, 1.0f  // top-left
    };
    glGenBuffers(1,&vboIds[0]);
    glBindBuffer(GL_ARRAY_BUFFER,vboIds[0]);
    glBufferData(GL_ARRAY_BUFFER,sizeof(verticesBig),verticesBig,GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER,0);
    LOGCATE("upload cost:%lld",(GetSysCurrentTime() - startTime));
    m_AngleX = 1;
    m_AngleY = 1;
}

void BlendSample::draw() {

    if(m_ProgramObj == GL_NONE || m_TextureId == GL_NONE) return;

    glClear(GL_STENCIL_BUFFER_BIT | GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glClearColor(1.0, 1.0, 1.0, 1.0);




//    GLushort indices[] = { 0, 1, 2, 0, 2, 3 };
    
//    LOGCATE("imagewidth:%i   ---imageHeight:%i",m_RenderImage.width,m_RenderImage.height);
    // Use the program object
    glUseProgram (m_ProgramObj);
    glEnable(GL_DEPTH_TEST);
//    glEnable(GL_BLEND);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
//    glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
//    glEnable(GL_DEPTH_TEST);

    glBindBuffer(GL_ARRAY_BUFFER,vboIds[0]);
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
    createMvp(false);


//    glActiveTexture(GL_TEXTURE1);
//    glBindTexture(GL_TEXTURE_2D, textures[0]);
//    // Set the RGBA map sampler to texture unit to 0
//    glUniform1i(m_SamplerLoc, 1);
//    createMvp(true);



}


void BlendSample::Destroy()
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
        glDeleteTextures(1, &m_TextureId);
        glDeleteTextures(4, textures);
        glDeleteBuffers(1,&vboIds[0]);
    }

}

void BlendSample::createMvp(bool isWindow) {
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
            glm::vec3( 1.5f,  0.0f, 0.0f),
            glm::vec3(-1.0f, 0.0f, 0.0f),
            glm::vec3(-1.5f, 0.0f, 0.0f),
//            glm::vec3( 2.4f, -0.4f, -3.5f),
//
//            glm::vec3(-1.7f,  3.0f, -7.5f),
//            glm::vec3( 1.3f, -2.0f, -2.5f),
//            glm::vec3( 1.5f,  2.0f, -2.5f),
//            glm::vec3( 1.5f,  0.2f, -1.5f),
//            glm::vec3(-1.3f,  1.0f, -1.5f)
    };
    if (isWindow){
        cubePositions[0] = glm::vec3 (0.0,0.0,0.1);
    }

    float value = mEyeZ + 1.0f;
    for(unsigned int i = 0; i < 1; i++)
    {
        UpdateMvp();
        setMat4(m_ProgramObj,"mMvpMatrix",mBaseMvpMatrix);
        glDrawArrays(GL_TRIANGLES, 0, 36);
    }




}

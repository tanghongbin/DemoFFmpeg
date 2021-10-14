//
// Created by Admin on 2020/8/25.
//


#include "TextureSample.h"
#include <GLES3/gl3.h>
#include "stb_image.h"
#include "CustomGLUtils.h"
#include "ImageDef.h"

void TextureSample::init(const char * vShaderStr,const char * fShaderStr) {

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
//    const char * filePath = "/storage/emulated/0/ffmpegtest/filterImg/test3.jpg";
    const char * filePath2 = "/storage/emulated/0/ffmpegtest/filterImg/leg.jpg";


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

    LoadImageInfo loadImageInfo;
    loadImageInfo.loadImage(filePath2);
    loadImageInfo.uploadImageTex2D();

//    glTexImage2D(GL_TEXTURE_2D, 0, format, localWidth,localHeight, 0, format, GL_UNSIGNED_BYTE, imageData);
//    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, m_RenderImage.width, m_RenderImage.height, 0, GL_RGBA, GL_UNSIGNED_BYTE,m_RenderImage.ppPlane[0]);
    glBindTexture(GL_TEXTURE_2D, GL_NONE);
    float verticesBig[] = {
            -1.0f, 1.0f, 0.0f,  0.0f, 0.0f,
            -1.0f, -1.0f, 0.0f,  0.0f, 1.0f,
            1.0f,  -1.0f, 0.0f,  1.0f, 1.0f,
            -1.0f,  1.0f, 0.0f,  0.0f, 0.0f,
            1.0f,  -1.0f, 0.0f,  1.0f, 1.0f,
            1.0f, 1.0f, 0.0f,  1.0f, 0.0f,

//            -1.0f, 1.0f,  0.0f,  0.0f, 1.0f,
//            1.0f, 1.0f,  0.0f,  1.0f, 1.0f,
//            1.0f,  -1.0f,  0.0f,  1.0f, 0.0f,
//            -1.0f, 1.0f,  0.0f,  0.0f, 1.0f,
//            1.0f,  -1.0f,  0.0f,  1.0f, 0.0f,
//            -1.0f, -1.0f,  0.0f,  0.0f, 0.0f,
    };
    glGenBuffers(1,&vboIds[0]);
    glBindBuffer(GL_ARRAY_BUFFER,vboIds[0]);
    glBufferData(GL_ARRAY_BUFFER,sizeof(verticesBig),verticesBig,GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER,0);
    LOGCATE("upload cost:%lld",(GetSysCurrentTime() - startTime));
}

void TextureSample::draw() {

    if(m_ProgramObj == GL_NONE || m_TextureId == GL_NONE) return;

    glClear(GL_STENCIL_BUFFER_BIT | GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glClearColor(1.0, 1.0, 1.0, 1.0);




//    GLushort indices[] = { 0, 1, 2, 0, 2, 3 };
    
//    LOGCATE("imagewidth:%i   ---imageHeight:%i",m_RenderImage.width,m_RenderImage.height);
    // Use the program object
    glUseProgram (m_ProgramObj);
    glEnable(GL_DEPTH_TEST);
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

    glDrawArrays(GL_TRIANGLES, 0, 6);
}


void TextureSample::Destroy()
{
    if (m_ProgramObj)
    {

        glDeleteProgram(m_ProgramObj);
        glDeleteTextures(1, &m_TextureId);
        glDeleteBuffers(1,&vboIds[0]);
    }

}

void TextureSample::createMvp() {





}

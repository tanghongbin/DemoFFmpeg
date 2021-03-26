//
// Created by Admin on 2020/8/25.
//


#include "TextureSample.h"
#include <GLES3/gl3.h>
#include "stb_image.h"
#include "CustomGLUtils.h"
#include "ImageDef.h"


void TextureSample::init(const char * vertexStr,const char * fragStr) {

    const char* vShaderStr =vertexStr;

    const char* fShaderStr =fragStr;
    m_ProgramObj = CreateProgram(vShaderStr, fShaderStr, m_VertexShader, m_FragmentShader);
    if (m_ProgramObj)
    {
        m_SamplerLoc = glGetUniformLocation(m_ProgramObj, "s_TextureMap");
    }
    else
    {
        LOGCATE("TextureMapSample::Init create program fail");
    }

    int64_t startTime= GetSysCurrentTime();
    int localWidth;
    int localHeight;
    int nrChannels;
    // test1,2,3 ...
    const char * filePath = "/storage/emulated/0/ffmpegtest/filterImg/test3.jpg";
    const char * filePath2 = "/storage/emulated/0/ffmpegtest/filterImg/test2.png";
    bool isPng = false;
    GLint format = GL_RGBA;
    if (!isPng){
        format = GL_RGB;
    }
    imageData = stbi_load(filePath, &localWidth, &localHeight, &nrChannels, 0);
    LOGCATE("image width:%d height:%d channel:%d data:%p costTime:%lld",localWidth,localHeight,nrChannels,imageData,
            (GetSysCurrentTime() - startTime));
    startTime = GetSysCurrentTime();
    //create RGBA texture
    glGenTextures(1, &m_TextureId);
    glBindTexture(GL_TEXTURE_2D, m_TextureId);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
//    glTexImage2D(GL_TEXTURE_2D, 0, format, localWidth,localHeight, 0, format, GL_UNSIGNED_BYTE, imageData);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, m_RenderImage.width, m_RenderImage.height, 0, GL_RGBA, GL_UNSIGNED_BYTE,m_RenderImage.ppPlane[0]);
    glBindTexture(GL_TEXTURE_2D, GL_NONE);
    LOGCATE("upload cost:%lld",(GetSysCurrentTime() - startTime));
    createMvp();
}

void TextureSample::draw() {

    if(m_ProgramObj == GL_NONE || m_TextureId == GL_NONE) return;

    glClear(GL_STENCIL_BUFFER_BIT | GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glClearColor(1.0, 1.0, 1.0, 1.0);

    GLfloat verticesCoords[] = {
            -1.0f,  0.5f, 0.0f,  // Position 0
            -1.0f, -0.5f, 0.0f,  // Position 1
            1.0f, -0.5f, 0.0f,   // Position 2
            1.0f,  0.5f, 0.0f,   // Position 3
    };

    GLfloat textureCoords[] = {
            0.0f,  0.0f,        // TexCoord 0
            0.0f,  1.0f,        // TexCoord 1
            1.0f,  1.0f,        // TexCoord 2
            1.0f,  0.0f         // TexCoord 3
    };

    GLushort indices[] = { 0, 1, 2, 0, 2, 3 };
    
//    LOGCATE("imagewidth:%i   ---imageHeight:%i",m_RenderImage.width,m_RenderImage.height);
    // Use the program object
    glUseProgram (m_ProgramObj);

    // Load the vertex position
    glVertexAttribPointer (0, 3, GL_FLOAT,
                           GL_FALSE, 0, verticesCoords);
    // Load the texture coordinate
    glVertexAttribPointer (1, 2, GL_FLOAT,
                           GL_FALSE, 0, textureCoords);

    glEnableVertexAttribArray (0);
    glEnableVertexAttribArray (1);

    // Bind the RGBA map
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, m_TextureId);

    // Set the RGBA map sampler to texture unit to 0
    glUniform1i(m_SamplerLoc, 0);

    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, indices);

}


void TextureSample::Destroy()
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
    }

}

void TextureSample::createMvp() {
    model = glm::rotate(model,glm::radians(-55.0f),glm::vec3(1.0f,0.0f,0.0f));
    view = glm::translate(view,glm::vec3(0.0f,0.0f,-3.0f));
    float ration = 1.3f;
    projection = glm::perspective(glm::radians(45.0f),ration,0.1f,100.0f);
    setMat4(m_ProgramObj, "model", model);
    setMat4(m_ProgramObj, "view", view);
    setMat4(m_ProgramObj, "projection", projection);
}

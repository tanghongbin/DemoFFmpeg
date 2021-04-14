//
// Created by Admin on 2020/8/25.
//


#include "InstanceSample.h"
#include <GLES3/gl3.h>
#include <camera.h>
#include <OpenGLImageDef.h>
#include "stb_image.h"
#include "CustomGLUtils.h"
#include "ImageDef.h"

void InstanceSample::init(const char * vShaderStr,const char * fShaderStr) {

    m_ProgramObj = CreateProgram(vShaderStr, fShaderStr, m_VertexShader, m_FragmentShader);
    if (m_ProgramObj)
    {
    }
    else
    {
        LOGCATE("TextureMapSample::Init create program fail");
    }




    float quadVertices[] = {
            // 位置          // 颜色
            -0.05f,  0.05f,  1.0f, 0.0f, 0.0f,
            0.05f, -0.05f,  0.0f, 1.0f, 0.0f,
            -0.05f, -0.05f,  0.0f, 0.0f, 1.0f,

            -0.05f,  0.05f,  1.0f, 0.0f, 0.0f,
            0.05f, -0.05f,  0.0f, 1.0f, 0.0f,
            0.05f,  0.05f,  0.0f, 1.0f, 1.0f
    };

    glm::vec2 translations[1000];
    int index = 0;
    float offset = 0.1f;
    for(int y = -10; y < 10; y += 2)
    {
        for(int x = -10; x < 10; x += 2)
        {
            glm::vec2 translation;
            translation.x = (float)x / 10.0f + offset;
            translation.y = (float)y / 10.0f + offset;
            translations[index++] = translation;
        }
    }

    glGenVertexArrays(4,vaoIds);
    glGenBuffers(4,vboIds);

    glBindBuffer(GL_ARRAY_BUFFER,vboIds[0]);
    glBufferData(GL_ARRAY_BUFFER,sizeof(quadVertices),quadVertices,GL_STATIC_DRAW);

    glBindBuffer(GL_ARRAY_BUFFER,vboIds[1]);
    glBufferData(GL_ARRAY_BUFFER,sizeof(translations) ,translations,GL_STATIC_DRAW);

    glBindVertexArray(vaoIds[0]);
    glBindBuffer(GL_ARRAY_BUFFER,vboIds[0]);
    // Load the vertex position
    glVertexAttribPointer (0, 2, GL_FLOAT,
                           GL_FALSE, 5 * sizeof(GL_FLOAT), (const void *)0);
    glEnableVertexAttribArray (0);

    glVertexAttribPointer (1, 3, GL_FLOAT,
                           GL_FALSE, 5 * sizeof(GL_FLOAT), (const void *)(3* sizeof(GLfloat)));
    glEnableVertexAttribArray (1);

    glBindBuffer(GL_ARRAY_BUFFER,vboIds[1]);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
    glEnableVertexAttribArray (2);
    glVertexAttribDivisor(2,1);
    glBindVertexArray(0);

    LOGCATE("upload cost:%lld",(GetSysCurrentTime() - startTime));
}


void InstanceSample::draw() {

    if(m_ProgramObj == GL_NONE) return;

    glClear(GL_STENCIL_BUFFER_BIT | GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glClearColor(1.0, 1.0, 1.0, 1.0);
    glEnable(GL_DEPTH_TEST);
    if (second == 0L){
        second = GetSysCurrentTime();
    }
    int64_t startTime = GetSysCurrentTime();
    float diffValue = (float)(GetSysCurrentTime() - second) / (float)1000;
//    LOGCATE("log second:%f",diffValue);
    glUseProgram (m_ProgramObj);
    glBindVertexArray(vaoIds[0]);
    glDrawArraysInstanced(GL_TRIANGLES, 0, 6,1000);
    LOGCATE("total cost:%lld",(GetSysCurrentTime() - startTime));
}

void InstanceSample::Destroy()
{
    if (m_ProgramObj)
    {
        LOGCATE("before over %p",imageData);
        if (imageData){
            stbi_image_free(imageData);
            LOGCATE("after over %p",imageData);
            imageData = nullptr;
        }
        glDeleteTextures(1,&m_TextureId);
        glDeleteProgram(m_ProgramObj);
        glDeleteVertexArrays(4,vaoIds);
        glDeleteBuffers(4,vboIds);
    }

}

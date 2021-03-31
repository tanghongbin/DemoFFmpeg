//
// Created by Admin on 2020/8/25.
//


#include "LightSample.h"
#include <GLES3/gl3.h>
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
            -0.5f, -0.5f, -0.5f,
            0.5f, -0.5f, -0.5f,
            0.5f,  0.5f, -0.5f,
            0.5f,  0.5f, -0.5f,
            -0.5f,  0.5f, -0.5f,
            -0.5f, -0.5f, -0.5f,

            -0.5f, -0.5f,  0.5f,
            0.5f, -0.5f,  0.5f,
            0.5f,  0.5f,  0.5f,
            0.5f,  0.5f,  0.5f,
            -0.5f,  0.5f,  0.5f,
            -0.5f, -0.5f,  0.5f,

            -0.5f,  0.5f,  0.5f,
            -0.5f,  0.5f, -0.5f,
            -0.5f, -0.5f, -0.5f,
            -0.5f, -0.5f, -0.5f,
            -0.5f, -0.5f,  0.5f,
            -0.5f,  0.5f,  0.5f,

            0.5f,  0.5f,  0.5f,
            0.5f,  0.5f, -0.5f,
            0.5f, -0.5f, -0.5f,
            0.5f, -0.5f, -0.5f,
            0.5f, -0.5f,  0.5f,
            0.5f,  0.5f,  0.5f,

            -0.5f, -0.5f, -0.5f,
            0.5f, -0.5f, -0.5f,
            0.5f, -0.5f,  0.5f,
            0.5f, -0.5f,  0.5f,
            -0.5f, -0.5f,  0.5f,
            -0.5f, -0.5f, -0.5f,

            -0.5f,  0.5f, -0.5f,
            0.5f,  0.5f, -0.5f,
            0.5f,  0.5f,  0.5f,
            0.5f,  0.5f,  0.5f,
            -0.5f,  0.5f,  0.5f,
            -0.5f,  0.5f, -0.5f,
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
                           GL_FALSE, 3 * sizeof(GL_FLOAT), (const void *)0);
    glEnableVertexAttribArray (0);
    glBindVertexArray(0);
    LOGCATE("upload cost:%lld",(GetSysCurrentTime() - startTime));
}

void LightSample::draw() {

    if(m_ProgramObj == GL_NONE) return;

    glClear(GL_STENCIL_BUFFER_BIT | GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glClearColor(0.0, 0.0, 0.0, 1.0);
    
//    LOGCATE("imagewidth:%i   ---imageHeight:%i",m_RenderImage.width,m_RenderImage.height);
    // Use the program object
    glUseProgram (m_ProgramObj);
    glEnable(GL_DEPTH_TEST);
    glBindVertexArray(lightVao);
    createMvp();


    glUseProgram (lightProgram);
    glEnable(GL_DEPTH_TEST);
    glBindVertexArray(lightVao);
    model = glm::mat4();
    view = glm::mat4(1.0f);
    projection = glm::mat4(1.0f);

    model = glm::translate(model, glm::vec3( 1.2f,  1.0f,  1.0f));
    model = glm::scale(model,  glm::vec3(0.2f));
    float angle = 20.0f;
    model = glm::rotate(model, 10.0f, glm::vec3(1.0f, 0.3f, 0.5f));
    view = glm::translate(view,glm::vec3(0.0f,0.0f,-5.0f));
    projection = glm::perspective(glm::radians(45.0f),1.33f,0.1f,100.0f);
    setMat4(lightProgram, "model", model);
    setMat4(lightProgram, "view", view);
    setMat4(lightProgram, "projection", projection);
    glDrawArrays(GL_TRIANGLES, 0, 36);
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
        glDeleteProgram(m_ProgramObj);
        glDeleteVertexArrays(1,&lightVao);
        glDeleteBuffers(1,&vboIds[0]);
    }

}

void LightSample::createMvp() {
    model = glm::mat4(1.0f);
    view = glm::mat4(1.0f);
    projection = glm::mat4(1.0f);

    model = glm::translate(model, glm::vec3( 0.0f,  0.0f,  0.0f));
    float angle = 20.0f;
    model = glm::rotate(model, 10.0f, glm::vec3(1.0f, 0.3f, 0.5f));
    view = glm::translate(view,glm::vec3(0.0f,0.0f,-5.0f));
    projection = glm::perspective(glm::radians(45.0f),1.33f,0.1f,100.0f);
    setMat4(m_ProgramObj, "model", model);
    setMat4(m_ProgramObj, "view", view);
    setMat4(m_ProgramObj, "projection", projection);
    setVec3(m_ProgramObj,"objectColor",glm::vec3(1.0f,0.5f,0.31f));
    setVec3(m_ProgramObj,"lightColor",glm::vec3(1.0f,1.0f,1.0f));
    glDrawArrays(GL_TRIANGLES, 0, 36);



}

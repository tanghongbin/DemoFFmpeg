//
// Created by Admin on 2020/8/25.
//


#include "LightSample.h"
#include <GLES3/gl3.h>
#include <camera.h>
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
            -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
            0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
            0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
            0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
            -0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
            -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,

            -0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,
            0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,
            0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,
            0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,
            -0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,
            -0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,

            -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,
            -0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
            -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
            -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
            -0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,
            -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,

            0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,
            0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
            0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
            0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
            0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,
            0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,

            -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,
            0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,
            0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
            0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
            -0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
            -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,

            -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,
            0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,
            0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
            0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
            -0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
            -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f
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
                           GL_FALSE, 6 * sizeof(GL_FLOAT), (const void *)0);
    glEnableVertexAttribArray (0);
    glVertexAttribPointer (1, 3, GL_FLOAT,
                           GL_FALSE, 6 * sizeof(GL_FLOAT), (const void *)(3* sizeof(GLfloat)));
    glEnableVertexAttribArray (1);
    glBindVertexArray(0);
    LOGCATE("upload cost:%lld",(GetSysCurrentTime() - startTime));
}

void LightSample::draw() {

    if(m_ProgramObj == GL_NONE) return;

    glClear(GL_STENCIL_BUFFER_BIT | GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glClearColor(0.0, 0.0, 0.0, 1.0);

    if (second == 0L){
        second = GetSysCurrentTime();
    }
    float diffValue = (float)(GetSysCurrentTime() - second) / (float)1000;
    LOGCATE("log second:%f",diffValue);
    glm::vec3 lightPos = glm::vec3( 1.2f,  1.3f,  3.0f);
    float radius = 8.0;
    float camX = sin(diffValue) * radius;
//    float camZ = sin(diffValue) * radius;
//    if (camZ <= 3.0){
//        camZ = 3.0;
//    }
    glm::vec3 cameraPos = glm::vec3(2.0,1.3f,6.0);
//    glm::vec3 cameraPos = glm::vec3(0.0,0.0f,3.0);
    glm::vec3 camreaFront = glm::vec3 (0.0,0.0,-1.0);
    glm::vec3 cameraUp    = glm::vec3(0.0f, 1.0f,  0.0f);
//    LOGCATE("imagewidth:%i   ---imageHeight:%i",m_RenderImage.width,m_RenderImage.height);
    // Use the program object
    Camera camera(cameraPos);

    glUseProgram (m_ProgramObj);
    glEnable(GL_DEPTH_TEST);
    glBindVertexArray(lightVao);
    setVec3(m_ProgramObj,"lightPos",lightPos);
    setVec3(m_ProgramObj,"viewPos",camera.Position);
    model = glm::mat4(1.0f);
    projection = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3( 0.0f,  0.0f,  0.0f));
//    model = glm::rotate(model, 5.0f, glm::vec3(1.0f, 0.3f, 0.0f));
    view = camera.GetViewMatrix();
    projection = glm::perspective(glm::radians(45.0f),1.33f,0.1f,100.0f);
    setMat4(m_ProgramObj, "model", model);
    setMat4(m_ProgramObj, "view", view);
    setMat4(m_ProgramObj, "projection", projection);
    setVec3(m_ProgramObj,"objectColor",glm::vec3(1.0f,0.5f,0.31f));
    setVec3(m_ProgramObj,"lightColor",glm::vec3(1.0f,1.0f,1.0f));
    setVec3(m_ProgramObj,"material.ambient",1.0f,0.5f,0.31f);
    setVec3(m_ProgramObj,"material.diffuse",1.0f,0.5f,0.31f);
    setVec3(m_ProgramObj,"material.specular",0.5,0.5,0.5);
    setFloat(m_ProgramObj,"material.shininess",32.0);
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

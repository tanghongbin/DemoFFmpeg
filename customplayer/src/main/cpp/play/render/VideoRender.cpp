//
// Created by Admin on 2021/5/13.
//

#include "../../play_header/render/VideoRender.h"
#include "../../play_header/model/shader.h"
#include "../../play_header/utils/CustomGLUtils.h"


void VideoRender::Init(){
    shader = new Shader(readGLSLStrFromFile("videoplay/vetex.glsl").c_str(),
            readGLSLStrFromFile("videoplay/fragment.glsl").c_str());

    GLfloat vetex[] = {
            -1.0f,1.0f,0.0f,
            -1.0f,-1.0f,0.0f,
            1.0f,-1.0f,0.0f,
            1.0f,-1.0f,0.0f,
            1.0f,1.0f,0.0f,
            -1.0f,1.0f,0.0f,
    };
    glGenBuffers(4,vboIds);
    glGenVertexArrays(2,vaoIds);
    glGenTextures(4,textures);
    glBindBuffer(GL_ARRAY_BUFFER,vboIds[0]);
    glBufferData(GL_ARRAY_BUFFER,sizeof(vetex),vetex,GL_STATIC_DRAW);

    glBindVertexArray(vaoIds[0]);
    glBindBuffer(GL_ARRAY_BUFFER,vboIds[0]);
    glVertexAttribPointer(0,3,GL_FLOAT,GL_FALSE,3 * sizeof(GL_FLOAT),(void *)0);
    glEnableVertexAttribArray(0);
    glBindVertexArray(0);
}

void VideoRender::DrawFrame() {

    glClearColor(1.0,1.0,1.0,1.0);
    glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
    shader->use();
    glBindVertexArray(vaoIds[0]);
    glDrawArrays(GL_TRIANGLES,0,6);
}

void VideoRender::Destroy() {
    glDeleteBuffers(4,vboIds);
    glDeleteVertexArrays(2,vaoIds);
    glDeleteTextures(4,textures);
}

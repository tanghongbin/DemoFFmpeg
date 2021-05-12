//
// Created by Admin on 2020/8/24.
//

#include "AntiAliasSample.h"


AntiAliasSample::AntiAliasSample() {

}

AntiAliasSample::~AntiAliasSample() {
    if (m_ProgramObj) {
        glDeleteProgram(m_ProgramObj);
    }

}

void AntiAliasSample::init(const char * vShaderStr,const char * fShaderStr) {


    mShader = new Shader(vShaderStr,fShaderStr);


    float verticesBig[] = {
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
            0.5f,  -0.5f, -0.5f,  1.0f, 0.0f,
            0.5f,  -0.5f,  0.5f,  1.0f, 0.0f,
            0.5f,  -0.5f, -0.5f,  1.0f, 0.0f,
            0.5f, 0.5f, 0.5f,  0.0f, 1.0f,
            0.5f, 0.5f, -0.5f,  0.0f, 1.0f,

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
    };
    glGenBuffers(2, vboIds);
    glBindBuffer(GL_ARRAY_BUFFER, vboIds[0]);
    glBufferData(GL_ARRAY_BUFFER,sizeof(verticesBig),verticesBig,GL_STATIC_DRAW);

    glGenVertexArrays(1,&vaoIds[0]);
    glBindVertexArray(vaoIds[0]);

    glBindBuffer(GL_ARRAY_BUFFER, vboIds[0]);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0,3,GL_FLOAT,GL_FALSE,sizeof(GLfloat) * 5,(const void *)0);
    glBindVertexArray(0);

}

void AntiAliasSample::draw() {

    if (m_ProgramObj == 0)
        return;

    glClear(GL_STENCIL_BUFFER_BIT | GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glClearColor(1.0, 1.0, 1.0, 1.0);

    // Use the program object
    mShader->use();
    UpdateMvp();
    mShader->setMat4("mvp",mBaseMvpMatrix);
    glBindVertexArray(vaoIds[0]);
    glDrawArrays(GL_TRIANGLES,0,36);
    glBindVertexArray(0);
}





void AntiAliasSample::Destroy() {
    glDeleteVertexArrays(1,&vaoIds[0]);
    glDeleteBuffers(2,vboIds);
}

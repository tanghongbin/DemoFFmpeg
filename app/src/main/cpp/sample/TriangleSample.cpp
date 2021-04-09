//
// Created by Admin on 2020/8/24.
//

#include "TriangleSample.h"


TriangleSample::TriangleSample() {

}

TriangleSample::~TriangleSample() {
    if (m_ProgramObj) {
        glDeleteProgram(m_ProgramObj);
    }

}

void TriangleSample::init(const char * vShaderStr,const char * fShaderStr) {


    GLuint m_VertexShader;
    GLuint m_FragmentShader;
    m_ProgramObj = CreateProgram(vShaderStr, fShaderStr, m_VertexShader, m_FragmentShader);
    int positionSize = 9;
    GLfloat vVertices[] = {
            -0.5f, -1.0f, 0.0f,
            -0.5f, 1.0f, 0.0f,
            1.0f, 1.0f, 0.0f
    };
    GLuint arrays[] = {
            0,1,2
    };
//    int size = 9;
    glGenBuffers(2, vboIds);
    glBindBuffer(GL_ARRAY_BUFFER, vboIds[0]);
    glBufferData(GL_ARRAY_BUFFER,sizeof(GLfloat) * positionSize,vVertices,GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,vboIds[1]);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER,sizeof(arrays),arrays,GL_STATIC_DRAW);

    glGenVertexArrays(1,&vaoIds[0]);
    glBindVertexArray(vaoIds[0]);

    glBindBuffer(GL_ARRAY_BUFFER, vboIds[0]);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,vboIds[1]);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0,3,GL_FLOAT,GL_FALSE,0,(const void *)NULL);
    glBindVertexArray(0);
    glDisableVertexAttribArray(0);

}

void TriangleSample::draw() {

    if (m_ProgramObj == 0)
        return;

    glClear(GL_STENCIL_BUFFER_BIT | GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glClearColor(1.0, 1.0, 1.0, 1.0);

    // Use the program object
    glUseProgram(m_ProgramObj);

    glBindVertexArray(vaoIds[0]);
    glDrawElements(GL_TRIANGLES,3,GL_UNSIGNED_INT,(const void *)0);
    glBindVertexArray(0);
}





void TriangleSample::Destroy() {
    glDeleteVertexArrays(1,&vboIds[0]);
}

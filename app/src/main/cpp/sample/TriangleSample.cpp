//
// Created by Admin on 2020/8/24.
//
#include "CustomGLUtils.h"
#include "TriangleSample.h"


TriangleSample::TriangleSample() {

}

TriangleSample::~TriangleSample() {
    if (m_ProgramObj) {
        glDeleteProgram(m_ProgramObj);
    }

}

void TriangleSample::init() {

    char vShaderStr[] =
            "#version 300 es                   \n"
            "layout(location = 1) in vec4 a_color;       \n"
            "layout(location = 0) in vec4 vPosition;  \n"
            "out vec4 v_color; \n"
            "void main()                              \n"
            "{                         \n"
            "   v_color = a_color;            \n"
            "   gl_Position = vPosition;              \n"
            "}                                        \n";

    char fShaderStr[] =
            "#version 300 es                              \n"
            "precision mediump float;   \n"
            "in vec4 v_color;                     \n"
            "out vec4 fragColor;                          \n"
            "void main()                                  \n"
            "{                                            \n"
            "   fragColor = vec4(1.0f,0.0f,0.0f,1.0f);  \n"
            "}                                            \n";

    GLuint m_VertexShader;
    GLuint m_FragmentShader;
    m_ProgramObj = CreateProgram(vShaderStr, fShaderStr, m_VertexShader, m_FragmentShader);
    GLfloat vVertices[] = {
            0.0f, 0.5f, 0.0f,
            -0.5f, -0.5f, 0.0f,
            0.5f, -0.5f, 0.0f,
    };

    GLfloat vVertices2[] = {
            -0.5f, 0.5f, 0.0f,
            -0.5f, -0.5f, 0.0f,
            0.5f, -0.5f, 0.0f,
    };
    glGenBuffers(2,vaoIds);
    glBindBuffer(GL_ARRAY_BUFFER,vaoIds[0]);
    glBufferData(GL_ARRAY_BUFFER,sizeof(GLfloat) * 9,vVertices,GL_STATIC_DRAW);

    glGenVertexArrays(1,&vaoIds[0]);
    glBindVertexArray(vaoIds[0]);

    GLuint offset = 0;
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (const void*)offset);
    glBindVertexArray(0);
}

void TriangleSample::draw() {
//    LOGCATE("TriangleSample::Draw");


//    GLfloat vVertices2[] = {
//            1.0f, 0.5f, 0.0f,
//            -0.5f, -0.5f, 0.0f,
//            0.5f, -0.5f, 0.0f,
//    };

    if (m_ProgramObj == 0)
        return;

    // Use the program object
    glUseProgram(m_ProgramObj);

    // Load the vertex data
    GLfloat colors1[12] = {1.0f,0.0f,0.0f,1.0f,
                          0.0f,1.0f,0.0f,1.0f,
                          0.0f,0.0f,1.0f,1.0f};
//    renderWithOriginal(vVertices, colors1);
    // use buffer
    renderWithBuffer(nullptr);

}

void TriangleSample::renderWithBuffer(const GLfloat *vVertices) const {
    glBindVertexArray(vaoIds[0]);
    glDrawArrays(GL_TRIANGLES,0,3);
    glBindVertexArray(0);
}

void TriangleSample::renderWithOriginal(const GLfloat *vVertices, const GLfloat *colors1) const {
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, vVertices);
    glEnableVertexAttribArray(0);


    glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 0, colors1);
    glEnableVertexAttribArray(1);


    glDrawArrays(GL_TRIANGLES, 0, 3);
    glDisableVertexAttribArray(0);
}

void TriangleSample::Destroy() {
    glDeleteVertexArrays(1,&vaoIds[0]);
}

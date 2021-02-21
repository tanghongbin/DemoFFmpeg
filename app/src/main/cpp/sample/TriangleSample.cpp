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
            "   fragColor = v_color;  \n"
            "}                                            \n";

    GLuint m_VertexShader;
    GLuint m_FragmentShader;
    m_ProgramObj = CreateProgram(vShaderStr, fShaderStr, m_VertexShader, m_FragmentShader);

}

void TriangleSample::draw() {
//    LOGCATE("TriangleSample::Draw");
    GLfloat vVertices[] = {
            -0.5f, 0.5f, 0.0f,
            -0.5f, -0.5f, 0.0f,
            0.5f, -0.5f, 0.0f,
    };

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
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, vVertices);
    glEnableVertexAttribArray(0);


    glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 0, colors1);
    glEnableVertexAttribArray(1);


    glDrawArrays(GL_TRIANGLES, 0, 3);
    glDisableVertexAttribArray(0);

}

void TriangleSample::Destroy() {

}

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
    GLfloat vVertices[] = {
            -0.5f, 0.0f, 0.0f,
            -0.5f, -0.5f, 0.0f,
            0.5f, -0.5f, 0.0f
    };
    int size = 9;
    glGenBuffers(2,vaoIds);
    glBindBuffer(GL_ARRAY_BUFFER,vaoIds[0]);
    glBufferData(GL_ARRAY_BUFFER,sizeof(GLfloat) * size,vVertices,GL_STATIC_DRAW);

    glGenVertexArrays(1,&vaoIds[0]);
    glBindVertexArray(vaoIds[0]);

    GLuint offset = 0;
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (const void*)offset);

    offset += sizeof(GLfloat) * size;
    GLfloat colors1[12] = {1.0f,0.0f,0.0f,1.0f,
                           0.0f,1.0f,0.0f,1.0f,
                           0.0f,0.0f,1.0f,1.0f};

    // bind color vetex
    glBindBuffer(GL_ARRAY_BUFFER,vaoIds[1]);
    glBufferData(GL_ARRAY_BUFFER,sizeof(GLfloat) * 12,colors1,GL_STATIC_DRAW);

//    glGenVertexArrays(1,&vaoIds[1]);
    glBindVertexArray(vaoIds[1]);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1,4,GL_FLOAT,GL_FALSE,0,(const void*)offset);
//    glVertexAttribDivisor(1,1);
    glBindVertexArray(0);
    mvpLoc = glGetUniformLocation(m_ProgramObj,"u_mvpMatrix");

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
    glBindVertexArray(vaoIds[1]);

    //转化为弧度角
//    float radiansX = static_cast<float>(MATH_PI / 180.0f * angleX);
//    float radiansY = static_cast<float>(MATH_PI / 180.0f * angleY);
//    // Projection matrix
//    glm::mat4 Projection = glm::ortho(-1.0f, 1.0f, -1.0f, 1.0f, 0.1f, 100.0f);
//    //glm::mat4 Projection = glm::frustum(-ratio, ratio, -1.0f, 1.0f, 4.0f, 100.0f);
//    //glm::mat4 Projection = glm::perspective(45.0f,ratio, 0.1f,100.f);
//
//    // View matrix
//    glm::mat4 View = glm::lookAt(
//            glm::vec3(0, 0, 4), // Camera is at (0,0,1), in World Space
//            glm::vec3(0, 0, 0), // and looks at the origin
//            glm::vec3(0, 1, 0)  // Head is up (set to 0,-1,0 to look upside-down)
//    );
//
//    // Model matrix
//    glm::mat4 Model = glm::mat4(1.0f);
//    Model = glm::scale(Model, glm::vec3(scaleX, scaleY, 1.0f));
//    Model = glm::rotate(Model, radiansX, glm::vec3(1.0f, 0.0f, 0.0f));
//    Model = glm::rotate(Model, radiansY, glm::vec3(0.0f, 1.0f, 0.0f));
//    Model = glm::translate(Model, glm::vec3(0.0f, 0.0f, 0.0f));

GLfloat values[3] = {1.0,1.0,1.0};
    glUniformMatrix4fv(mvpLoc,1,GL_FALSE,values);
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

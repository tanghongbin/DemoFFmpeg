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

void TriangleSample::init() {

    char vShaderStr[] =
            "#version 300 es\n"
            "uniform mat4 m_MVPMatrix;\n"
            "layout(location = 0) in vec4 vPosition;\n"
            "layout(location = 1) in vec4 a_color;\n"
            "out vec4 v_color; \n"
            "void main()                              \n"
            "{                         \n"
            "   v_color = a_color;            \n"
            "   gl_Position = vPosition;\n"
            "}";

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
            -0.5f, 0.0f, 0.0f,
            -0.5f, -0.5f, 0.0f,
            0.5f, -0.5f, 0.0f
    };
//    int size = 9;
//    glGenBuffers(2,vaoIds);
//    glBindBuffer(GL_ARRAY_BUFFER,vaoIds[0]);
//    glBufferData(GL_ARRAY_BUFFER,sizeof(GLfloat) * size,vVertices,GL_STATIC_DRAW);
//
//    glGenVertexArrays(1,&vaoIds[0]);
//    glBindVertexArray(vaoIds[0]);
//
//    GLuint offset = 0;
//    glEnableVertexAttribArray(0);
//    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (const void*)offset);
//
//    offset += sizeof(GLfloat) * size;
//    GLfloat colors1[12] = {1.0f,0.0f,0.0f,1.0f,
//                           0.0f,1.0f,0.0f,1.0f,
//                           0.0f,0.0f,1.0f,1.0f};
//
//    // bind color vetex
//    glBindBuffer(GL_ARRAY_BUFFER,vaoIds[1]);
//    glBufferData(GL_ARRAY_BUFFER,sizeof(GLfloat) * 12,colors1,GL_STATIC_DRAW);
//
////    glGenVertexArrays(1,&vaoIds[1]);
//    glBindVertexArray(vaoIds[1]);
//    glEnableVertexAttribArray(1);
//    glVertexAttribPointer(1,4,GL_FLOAT,GL_FALSE,0,(const void*)offset);
////    glVertexAttribDivisor(1,1);
//    glBindVertexArray(0);

// texture
    GLubyte  gLubyte[4 * 3] = {
            255,0,0,
            0,255,0,
            0,0,255,
            255,255,0
    };
    GLuint textures[1] = {1};
    glGenTextures(1,textures);
    glBindTexture(GL_TEXTURE_2D,textures[0]);
    glTexImage2D(GL_TEXTURE_2D,0,GL_RGB,2,2,0,GL_RGB,GL_UNSIGNED_BYTE,gLubyte);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_NEAREST);
    glBindTexture(GL_TEXTURE_2D,GL_NONE);
}

void TriangleSample::draw() {
//    LOGCATE("TriangleSample::Draw");


    GLfloat vVertices2[] = {
            -1.0f, 1.0f, 0.0f,
            -1.0f, -1.0f, 0.0f,
            1.0f, -1.0f, 0.0f,
    };

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
    renderWithBuffer(vVertices2);

}

void TriangleSample::renderWithBuffer(const GLfloat *vVertices) {
//    glBindVertexArray(vaoIds[0]);
//    glBindVertexArray(vaoIds[1]);


    UpdateMVPMatrix(1,1,1.0f,1.0f);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, vVertices);
    glEnableVertexAttribArray(0);
    setMat4(m_ProgramObj,"u_MVPMatrix",m_MVPMatrix);
    glActiveTexture(1);
    glBindTexture(GL_TEXTURE_2D,1);
//    glDrawArrays(GL_TRIANGLES,0,3);
    GLuint indices[3] = {0,1,2};
    glDrawElements(GL_TRIANGLES,3,GL_UNSIGNED_INT,indices);
//    glBindVertexArray(0);
}


void TriangleSample::UpdateMVPMatrix(int angleX, int angleY, float scaleX, float scaleY)
{
    angleX = angleX % 360;
    angleY = angleY % 360;

    //转化为弧度角
    float radiansX = static_cast<float>(MATH_PI / 180.0f * angleX);
    float radiansY = static_cast<float>(MATH_PI / 180.0f * angleY);
    // Projection matrix
    glm::mat4 Projection = glm::ortho(-1.0f, 1.0f, -1.0f, 1.0f, 0.1f, 100.0f);
    //glm::mat4 Projection = glm::frustum(-ratio, ratio, -1.0f, 1.0f, 4.0f, 100.0f);
    //glm::mat4 Projection = glm::perspective(45.0f,ratio, 0.1f,100.f);

    // View matrix
    glm::mat4 View = glm::lookAt(
            glm::vec3(0, 0, 4), // Camera is at (0,0,1), in World Space
            glm::vec3(0, 0, 0), // and looks at the origin
            glm::vec3(0, 1, 0)  // Head is up (set to 0,-1,0 to look upside-down)
    );

    // Model matrix
    glm::mat4 Model = glm::mat4(1.0f);
    Model = glm::scale(Model, glm::vec3(scaleX, scaleY, 1.0f));
    Model = glm::rotate(Model, radiansX, glm::vec3(1.0f, 0.0f, 0.0f));
    Model = glm::rotate(Model, radiansY, glm::vec3(0.0f, 1.0f, 0.0f));
    Model = glm::translate(Model, glm::vec3(0.0f, 0.0f, 0.0f));

    m_MVPMatrix = Projection * View * Model;

}

void TriangleSample::renderWithOriginal(const GLfloat *vVertices, const GLfloat *colors1)  {
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, vVertices);
    glEnableVertexAttribArray(0);


    glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 0, colors1);
    glEnableVertexAttribArray(1);


    glDrawArrays(GL_TRIANGLES, 0, 6);
    glDisableVertexAttribArray(0);
}

void TriangleSample::Destroy() {
    glDeleteVertexArrays(1,&vaoIds[0]);
}

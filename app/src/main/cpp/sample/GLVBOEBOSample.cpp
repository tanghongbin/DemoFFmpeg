//
// Created by Admin on 2020/8/26.
//

#include <CustomGLUtils.h>
#include "GLVBOEBOSample.h"

/**
 * 废弃不使用，缘由是顶点和片段着色器编译有问题
 */
void GLVBOEBOSample::init() {

    char vShaderStr[] = "#version 300 es                            \n"
                        "layout(location = 0) in vec4 a_position;   // 位置变量的属性位置值为 0 \n"
                        "layout(location = 1) in vec3 a_color;      // 颜色变量的属性位置值为 1\n"
                        "out vec3 v_color;                          // 向片段着色器输出一个颜色                          \n"
                        "void main()                                \n"
                        "{                                          \n"
                        "    v_color = a_color;                     \n"
                        "    gl_Position = a_position;              \n"
                        "};";

    char fShaderStr[] = "#version 300 es\n"
                        "precision mediump float;\n"
                        "in vec3 v_color;\n"
                        "out vec4 o_fragColor;\n"
                        "void main()\n"
                        "{\n"
                        "    o_fragColor = vec4(v_color, 1.0);\n"
                        "}";

    m_ProgramObj = CreateProgram(vShaderStr, fShaderStr, m_VertexShader, m_FragmentShader);

    // 创建 2 个 VBO（EBO 实际上跟 VBO 一样，只是按照用途的另一种称呼）
    glGenBuffers(2, m_VboIds);

    // 4 vertices, with(x,y,z) ,(r, g, b, a) per-vertex
    GLfloat vertices[] =
            {
                    -0.5f,  0.5f, 0.0f,       // v0
                    1.0f,  0.0f, 0.0f,        // c0
                    -0.5f, -0.5f, 0.0f,       // v1
                    0.0f,  1.0f, 0.0f,        // c1
                    0.5f, -0.5f, 0.0f,        // v2
                    0.0f,  0.0f, 1.0f,        // c2
                    0.5f,  0.5f, 0.0f,        // v3
                    0.5f,  1.0f, 1.0f,        // c3
            };
// Index buffer data
    GLushort indices[6] = { 0, 1, 2, 0, 2, 3};

// 绑定第一个 VBO，拷贝顶点数组到显存
    glBindBuffer(GL_ARRAY_BUFFER, m_VboIds[0]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

// 绑定第二个 VBO（EBO），拷贝图元索引数据到显存
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_VboIds[1]);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

}

void GLVBOEBOSample::draw() {
    glUseProgram(m_ProgramObj);

//使用 VBO 的绘制
    glBindBuffer(GL_ARRAY_BUFFER, m_VboIds[0]);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, (3+3)*sizeof(GLfloat), (const void *)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, (3+3)*sizeof(GLfloat), (const void *)(3 *sizeof(GLfloat)));

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_VboIds[1]);

    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, (const void *)0);
}

void GLVBOEBOSample::Destroy()
{
    if (m_ProgramObj)
    {
        glDeleteProgram(m_ProgramObj);
    }

}
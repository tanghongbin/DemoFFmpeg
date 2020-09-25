//
// Created by Admin on 2020/8/28.
//

#include <GLUtils.h>
#include <MyGLRenderContext.h>
#include "CoordinateSystemSample.h"


CoordinateSystemSample::CoordinateSystemSample()
{

    m_SamplerLoc = GL_NONE;
    m_MVPMatLoc = GL_NONE;

    m_TextureId = GL_NONE;
    m_VaoId = GL_NONE;

    m_AngleX = 0;
    m_AngleY = 0;

    m_ScaleX = 1.0f;
    m_ScaleY = 1.0f;
}

CoordinateSystemSample::~CoordinateSystemSample()
{
    NativeImageUtil::FreeNativeImage(&m_RenderImage);
}

void CoordinateSystemSample::init() {
    if(m_ProgramObj)
        return;
    //create RGBA texture
    glGenTextures(1, &m_TextureId);
    glBindTexture(GL_TEXTURE_2D, m_TextureId);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glBindTexture(GL_TEXTURE_2D, GL_NONE);

    char vShaderStr[] = "#version 300 es\n"
                        "precision mediump float;\n"
                        "layout(location = 0) in vec4 a_position;\n"
                        "layout(location = 1) in vec2 a_texCoord;\n"
                        "layout(location = 2) in vec3 a_normal;\n"
                        "uniform mat4 u_MVPMatrix;\n"
                        "uniform mat4 u_ModelMatrix;\n"
                        "uniform vec3 lightPos;\n"
                        "uniform vec3 lightColor;\n"
                        "uniform vec3 viewPos;\n"
                        "out vec2 v_texCoord;\n"
                        "out vec3 ambient;\n"
                        "out vec3 diffuse;\n"
                        "out vec3 specular;\n"
                        "void main()\n"
                        "{\n"
                        "gl_Position = u_MVPMatrix * a_position;\n"
                        "vec3 fragPos = vec3(u_ModelMatrix * a_position);\n"
                        "\n"
                        "// Ambient\n"
                        "float ambientStrength = 0.1;\n"
                        "ambient = ambientStrength * lightColor;\n"
                        "\n"
                        "// Diffuse\n"
                        "float diffuseStrength = 0.5;\n"
                        "vec3 unitNormal = normalize(vec3(u_ModelMatrix * vec4(a_normal, 1.0)));\n"
                        "vec3 lightDir = normalize(lightPos - fragPos);\n"
                        "float diff = max(dot(unitNormal, lightDir), 0.0);\n"
                        "diffuse = diffuseStrength * diff * lightColor;\n"
                        "\n"
                        "// Specular\n"
                        "float specularStrength = 0.9;\n"
                        "vec3 viewDir = normalize(viewPos - fragPos);\n"
                        "vec3 reflectDir = reflect(-lightDir, unitNormal);\n"
                        "float spec = pow(max(dot(unitNormal, reflectDir), 0.0), 16.0);\n"
                        "specular = specularStrength * spec * lightColor;\n"
                        "v_texCoord = a_texCoord;\n"
                        "}";

    char fShaderStr[] = "#version 300 es\n"
                        "precision mediump float;\n"
                        "in vec2 v_texCoord;\n"
                        "in vec3 ambient;\n"
                        "in vec3 diffuse;\n"
                        "in vec3 specular;\n"
                        "layout(location = 0) out vec4 outColor;\n"
                        "uniform sampler2D s_TextureMap;\n"
                        "void main()\n"
                        "{\n"
                        "vec4 objectColor = texture(s_TextureMap, v_texCoord);\n"
                        "vec3 finalColor = (ambient + diffuse + specular) * vec3(objectColor);\n"
                        "outColor = vec4(finalColor, 1.0);\n"
                        "}";

    m_ProgramObj = CreateProgram(vShaderStr, fShaderStr, m_VertexShader, m_FragmentShader);
    if (m_ProgramObj)
    {
        m_SamplerLoc = glGetUniformLocation(m_ProgramObj, "s_TextureMap");
        m_MVPMatLoc = glGetUniformLocation(m_ProgramObj, "u_MVPMatrix");
    }
    else
    {
        LOGCATE("CoordSystemSample::Init create program fail");
    }

    GLfloat verticesCoords[] = {
            -1.0f,  1.0f, 0.0f,  // Position 0
            -1.0f, -1.0f, 0.0f,  // Position 1
            1.0f,  -1.0f, 0.0f,  // Position 2
            1.0f,   1.0f, 0.0f,  // Position 3
    };

    GLfloat textureCoords[] = {
            0.0f,  0.0f,        // TexCoord 0
            0.0f,  1.0f,        // TexCoord 1
            1.0f,  1.0f,        // TexCoord 2
            1.0f,  0.0f         // TexCoord 3
    };

    GLushort indices[] = { 0, 1, 2, 0, 2, 3 };

    // Generate VBO Ids and load the VBOs with data
    glGenBuffers(3, m_VboIds);
    glBindBuffer(GL_ARRAY_BUFFER, m_VboIds[0]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(verticesCoords), verticesCoords, GL_STATIC_DRAW);

    glBindBuffer(GL_ARRAY_BUFFER, m_VboIds[1]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(textureCoords), textureCoords, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_VboIds[2]);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    // Generate VAO Id
    glGenVertexArrays(1, &m_VaoId);
    glBindVertexArray(m_VaoId);

    glBindBuffer(GL_ARRAY_BUFFER, m_VboIds[0]);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (const void *)0);
    glBindBuffer(GL_ARRAY_BUFFER, GL_NONE);

    glBindBuffer(GL_ARRAY_BUFFER, m_VboIds[1]);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(GLfloat), (const void *)0);
    glBindBuffer(GL_ARRAY_BUFFER, GL_NONE);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_VboIds[2]);

    glBindVertexArray(GL_NONE);

    //upload RGBA image data
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, m_TextureId);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, m_RenderImage.width, m_RenderImage.height, 0, GL_RGBA, GL_UNSIGNED_BYTE, m_RenderImage.ppPlane[0]);
    glBindTexture(GL_TEXTURE_2D, GL_NONE);
}

void CoordinateSystemSample::draw() {

    if(m_ProgramObj == GL_NONE || m_TextureId == GL_NONE) return;
    m_AngleX++;
    m_AngleY++;
    m_AngleX = m_AngleX > 360 ? 0 : m_AngleX;
    m_AngleY = m_AngleY > 360 ? 0 : m_AngleY;


    UpdateMVPMatrix(m_MVPMatrix, m_AngleX, m_AngleY, (float)MyGLRenderContext::GetInstance()->width / MyGLRenderContext::GetInstance()->height);

    // Use the program object
    glUseProgram (m_ProgramObj);

    glBindVertexArray(m_VaoId);

    glUniformMatrix4fv(m_MVPMatLoc, 1, GL_FALSE, &m_MVPMatrix[0][0]);

    // Bind the RGBA map
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, m_TextureId);
    glUniform1i(m_SamplerLoc, 0);

    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, (const void *)0);

}

void CoordinateSystemSample::UpdateMVPMatrix(glm::mat4 &mvpMatrix, int angleX, int angleY, float ratio)
{
    angleX = angleX % 360;
    angleY = angleY % 360;

    //转化为弧度角
    float radiansX = static_cast<float>(MATH_PI / 180.0f * angleX);
    float radiansY = static_cast<float>(MATH_PI / 180.0f * angleY);


    // Projection matrix
    //glm::mat4 Projection = glm::ortho(-1.0f, 1.0f, -1.0f, 1.0f, 0.1f, 100.0f);
    //glm::mat4 Projection = glm::frustum(-ratio, ratio, -1.0f, 1.0f, 4.0f, 100.0f);
    glm::mat4 Projection = glm::perspective(45.0f,ratio, 0.1f,100.f);

    // View matrix
    glm::mat4 View = glm::lookAt(
            glm::vec3(0, 0, 4), // Camera is at (0,0,1), in World Space
            glm::vec3(0, 0, 0), // and looks at the origin
            glm::vec3(0, 1, 0)  // Head is up (set to 0,-1,0 to look upside-down)
    );

    // Model matrix
    glm::mat4 Model = glm::mat4(1.0f);
    Model = glm::scale(Model, glm::vec3(m_ScaleX, m_ScaleY, 1.0f));
    Model = glm::rotate(Model, radiansX, glm::vec3(1.0f, 0.0f, 0.0f));
    Model = glm::rotate(Model, radiansY, glm::vec3(0.0f, 1.0f, 0.0f));
    Model = glm::translate(Model, glm::vec3(0.0f, 0.0f, 0.0f));

    mvpMatrix = Projection * View * Model;

}

void CoordinateSystemSample::Destroy() {
    if (m_ProgramObj)
    {
        glDeleteProgram(m_ProgramObj);
        glDeleteBuffers(3, m_VboIds);
        glDeleteVertexArrays(1, &m_VaoId);
        glDeleteTextures(1, &m_TextureId);
    }
}
//
// Created by Admin on 2020/8/25.
//

#ifndef DEMOC_SHADOWSAMPLESAMPLE_H
#define DEMOC_SHADOWSAMPLESAMPLE_H

#include "GLES3/gl3.h"
#include "GLBaseSample.h"
#include <CustomGLUtils.h>

class ShadowSample : public GLBaseSample{

private:
    GLuint m_ProgramObj;
    GLuint vboIds[4];
    GLuint vaoIds[4];
    GLuint mTextureIds[2];
    glm::mat4 m_MVPMatrix;
    Shader* lightShader = 0;
    Shader* shadowShader = 0;
    GLuint depthTexture;
    const GLuint SHADOW_WIDTH = 1024, SHADOW_HEIGHT = 1024;

public:
    ShadowSample();

    ~ShadowSample();

    void init(const char * vertexStr,const char * fragStr);

    void init();

    void draw();

    void Destroy();

    void generateFbo();

    void renderScene(Shader *pShader);

    void ConfigureShaderAndMatrices();
};

#endif //DEMOC_SHADOWSAMPLESAMPLE_H

//
// Created by Admin on 2020/8/25.
//

#ifndef DEMOC_SHADOWCUBESAMPLESAMPLE_H
#define DEMOC_SHADOWCUBESAMPLESAMPLE_H

#include "GLES3/gl3.h"
#include "GLBaseSample.h"
#include <CustomGLUtils.h>

class ShadowCubeSample : public GLBaseSample{

private:
    GLuint m_ProgramObj;
    GLuint vboIds[4];
    GLuint vaoIds[4];
    GLuint mTextureIds[2];
    glm::mat4 m_MVPMatrix;
    Shader* mLightShader = 0;
    Shader* mShadowShader = 0;
    GLuint depthCubeTexture;
    const GLuint SHADOW_WIDTH = 1024, SHADOW_HEIGHT = 1024;

    glm::vec3 models [3] = {
            glm::vec3 (-0.7f, 0.5f,0.0f),
            glm::vec3 (0.5f,0.0f,0.0f),
            glm::vec3 (0.3f,0.0f,0.3f),
    };

public:
    ShadowCubeSample();

    ~ShadowCubeSample();

    void init(const char * vertexStr,const char * fragStr);

    void init();

    void draw();

    void Destroy();

    void generateFbo();

    void renderScene(Shader *pShader);

    void renderSimpleFbo(Shader *pShader);

};

#endif //DEMOC_SHADOWCUBESAMPLESAMPLE_H

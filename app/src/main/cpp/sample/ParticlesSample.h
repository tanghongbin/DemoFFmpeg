//
// Created by Admin on 2020/9/1.
//

#ifndef DEMOC_PARTICLESSAMPLE_H
#define DEMOC_PARTICLESSAMPLE_H

#define MAX_PARTICLES 999


#include <detail/type_mat.hpp>
#include "GLBaseSample.h"

struct Particle {
    GLfloat dx,dy,dz,cameraDistance;//offset 控制粒子的位置
    GLfloat dxSpeed,dySpeed,dzSpeed;//speed 控制粒子的运动速度
    GLubyte r,g,b,a; //r,g,b,a 控制粒子的颜色
    GLfloat life; //控制粒子的生命值
    Particle()
    {
        dx = 0.0f;
        dy = 0.0f;
        dz = 0.0f;

        r = static_cast<GLubyte>(1.0f);
        g = static_cast<GLubyte>(1.0f);
        b = static_cast<GLubyte>(1.0f);
        a = static_cast<GLubyte>(1.0f);

        dxSpeed = 1.0f;
        dySpeed = 1.0f;
        dzSpeed = 1.0f;

        life = 0.0f;
    }
};

class ParticlesSample : public GLBaseSample {


public:
    ParticlesSample();

    ~ParticlesSample();

    void init();

    void draw();

    void Destroy();

    void GenerateNewParticle(Particle &particle);

    virtual void UpdateTransformMatrix(float rotateX, float rotateY, float scaleX, float scaleY);

    void UpdateMVPMatrix(glm::mat4 &mvpMatrix, int angleX, int angleY, float ratio);

    int FindUnusedParticle();

    void SortParticles();

    int UpdateParticles();

private:
    GLuint m_TextureId;
    GLint m_SamplerLoc;
    GLint m_MVPMatLoc;

    GLuint m_VaoId;
    GLuint m_ParticlesVertexVboId;
    GLuint m_ParticlesPosVboId;
    GLuint m_ParticlesColorVboId;

    NativeImage m_RenderImage;
    glm::mat4 m_MVPMatrix;

    // particles relation
    Particle m_ParticlesContainer[MAX_PARTICLES];
    GLfloat* m_pParticlesPosData;
    GLubyte* m_pParticlesColorData;
    int m_LastUsedParticle;

    int m_AngleX;
    int m_AngleY;

    float m_ScaleX;
    float m_ScaleY;

};


#endif //DEMOC_PARTICLESSAMPLE_H

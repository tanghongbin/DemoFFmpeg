//
// Created by Admin on 2020/8/26.
//

#ifndef DEMOC_GLBASESAMPLE_H
#define DEMOC_GLBASESAMPLE_H

#include <GLES3/gl3.h>
#include "../utils/ImageDef.h"
#include <detail/type_mat.hpp>
#include <gtc/type_ptr.hpp> // glm::value_ptr
#include <gtc/matrix_transform.hpp>

class GLBaseSample {

protected:

    GLuint m_ProgramObj;
    GLuint m_TextureId;
    GLuint m_fboTextureId;
    GLuint m_fboId;
    GLuint m_VertexShader;
    GLuint m_FragmentShader;
    GLint m_SamplerLoc;
    NativeImage m_RenderImage;
    int64_t startTime = 0L;
    glm::mat4 mBaseMvpMatrix;
    glm::mat4 mBaseModel;
    glm::mat4 mBaseProjection;
    glm::mat4 mBaseView;

    int m_AngleX = 0;
    int m_AngleY = 0 ;
    float m_ScaleX = 1.0f;
    float m_ScaleY = 1.0f;

    float mEyeZ = 3.0f;


public:

    GLBaseSample() {

    };

    virtual ~GLBaseSample() {

    };

    int screenWidth, screenHeight;

    virtual void init(const char * vertexStr,const char * fragStr) = 0;

    void init() {

    }

    virtual void draw() = 0;

    void loadImage(NativeImage *pImage) {
        LOGCATE("TextureMapSample::LoadImage pImage = %p ", pImage->ppPlane[0]);
        LOGCATE("TextureMapSample::m_RenderImage pImage = %p ", &m_RenderImage);
        m_RenderImage.width = pImage->width;
        m_RenderImage.height = pImage->height;
        m_RenderImage.format = pImage->format;
        NativeImageUtil::CopyNativeImage(pImage, &m_RenderImage);
        LOGCATE("TextureMapSample::LoadImage pImage over");
    }

    void GO_CHECK_GL_ERROR() {
        LOGCATE("nothing to check");
    }

    virtual void Destroy() = 0;

    // 1-减少，2-增加
    virtual void ChangeEyeZValue(int type,float zValue){
        if (type == 1){
            mEyeZ -= zValue;
        } else {
            mEyeZ += zValue;
        }
//        LOGCATE("currentValue:%f",mEyeZ);
    }

    virtual void UpdateTransformMatrix(float rotateX, float rotateY, float scaleX,
                                       float scaleY) {
        m_AngleX = static_cast<int>(rotateX);
        m_AngleY = static_cast<int>(rotateY);
        m_ScaleX = scaleX;
        m_ScaleY = scaleY;
        LOGCATE("log anglex:%d angleY:%d scalex:%f scaleY:%f",m_AngleX,m_AngleY,m_ScaleX,m_ScaleY);
    }

    virtual void UpdateMvp(){
        float angleX = m_AngleX % 360;
        float angleY = m_AngleY % 360;

        //转化为弧度角
        float radiansX = static_cast<float>(MATH_PI / 180.0f * angleX);
        float radiansY = static_cast<float>(MATH_PI / 180.0f * angleY);


        // Projection matrix
//glm::mat4 Projection = glm::ortho(-ratio, ratio, -1.0f, 1.0f, 0.0f, 100.0f);
//glm::mat4 Projection = glm::frustum(-ratio, ratio, -1.0f, 1.0f, 4.0f, 100.0f);
        glm::mat4 Projection = glm::perspective(45.0f, (float )screenWidth/(float )screenHeight, 0.1f, 100.f);

        // View matrix
        glm::mat4 View = glm::lookAt(
                glm::vec3(-3, 0, 3), // Camera is at (0,0,1), in World Space
                glm::vec3(0, 0, 0), // and looks at the origin
                glm::vec3(0, 1, 0)  // Head is up (set to 0,-1,0 to look upside-down)
        );

        // Model matrix
        glm::mat4 Model = glm::mat4(1.0f);
        Model = glm::scale(Model, glm::vec3(m_ScaleX, m_ScaleX, m_ScaleX));
        Model = glm::rotate(Model, radiansX, glm::vec3(1.0f, 0.0f, 0.0f));
        Model = glm::rotate(Model, radiansY, glm::vec3(0.0f, 1.0f, 0.0f));
        Model = glm::translate(Model, glm::vec3(0.0f, 0.0f, 0.0f));

        mBaseModel = Model;
        mBaseView = View;
        mBaseProjection = Projection;

        mBaseMvpMatrix = Projection * View * Model;
    }

};

#endif //DEMOC_GLBASESAMPLE_H

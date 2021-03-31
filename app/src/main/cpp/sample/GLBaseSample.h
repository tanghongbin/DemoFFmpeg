//
// Created by Admin on 2020/8/26.
//

#ifndef DEMOC_GLBASESAMPLE_H
#define DEMOC_GLBASESAMPLE_H

#include <GLES3/gl3.h>
#include "../utils/ImageDef.h"

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

};

#endif //DEMOC_GLBASESAMPLE_H

//
// Created by Admin on 2020/9/28.
//

#ifndef DEMOC_OPENGLFFMPEGRENDER_H
#define DEMOC_OPENGLFFMPEGRENDER_H


#include <__mutex_base>
#include <GLES3/gl3.h>
#include <detail/type_mat.hpp>
#include <GLBaseSample.h>
#include "VideoRenderInterface.h"
#include "OpenGLImageDef.h"

class OpenGLFFmpegRender {

private:
    OpenGLFFmpegRender();

    ~OpenGLFFmpegRender();

    static std::mutex m_Mutex;
    static OpenGLFFmpegRender *s_Instance;
    NativeOpenGLImage m_RenderImage;
    GLuint m_ProgramObj;
    GLuint m_TextureId;
    GLuint m_VertexShader;
    GLuint m_FragmentShader;
    GLint m_SamplerLoc;

public:

    static int mWindowWidth;

    static int mWindowHeight;

    static OpenGLFFmpegRender *getInstance();

    static void destroyInstance();

    void init(int width, int height);

    void unInit();

    void onSurfaceCreated();

    void onDrawFrame();

    void onSurfaceChanged(int width, int height);

    void SetImageData(int format, int width, int height, uint8_t *pData);

    void setupImage(NativeOpenGLImage* image);

};


#endif //DEMOC_OPENGLFFMPEGRENDER_H

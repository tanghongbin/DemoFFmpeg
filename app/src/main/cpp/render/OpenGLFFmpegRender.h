//
// Created by Admin on 2020/9/28.
//

#ifndef DEMOC_OPENGLFFMPEGRENDER_H
#define DEMOC_OPENGLFFMPEGRENDER_H


#include <__mutex_base>
#include <GLES3/gl3.h>
#include <detail/type_mat.hpp>
#include "VideoRenderInterface.h"

class OpenGLFFmpegRender : public VideoRenderInterface {

private:
    OpenGLFFmpegRender();

    ~OpenGLFFmpegRender();

    static std::mutex m_Mutex;
    static OpenGLFFmpegRender *s_Instance;
    GLuint m_ProgramObj = GL_NONE;
    GLuint m_TextureId;
    GLuint m_VaoId;
    GLuint m_VboIds[3];
    NativeImage m_RenderImage;
//    glm::mat4 m_MVPMatrix;//变换矩阵
    int mFrameIndex;
    GLint m_SamplerLoc;
    GLuint m_VertexShader,m_FragmentShader;

public:
    static OpenGLFFmpegRender* getInstance();

    static void destroyInstance();

    void init(int width, int height);

    void render_video(NativeImage *nativeImage);

    void unInit();

    void onSurfaceCreated();

    void onDrawFrame();

    void onSurfaceChanged(int width, int height);

    void UpdateMVPMatrix(int angleX, int angleY, float scaleX, float scaleY);

    void SetImageData(int format, int width, int height, uint8_t *pData);


};


#endif //DEMOC_OPENGLFFMPEGRENDER_H

//
// Created by Admin on 2021/5/13.
//

#ifndef DEMOFFMPEG_FBORENDER_H
#define DEMOFFMPEG_FBORENDER_H

#include "BaseRender.h"
#include "../../play_header/render/VideoFboRender.h"
#include "../../play_header/model/shader.h"
#include "../../play_header/utils/CustomGLUtils.h"
#include <GLES3/gl3ext.h>
#include <EGL/egl.h>
#include <EGL/eglext.h>
#include <android/hardware_buffer.h>

extern "C" {
#include <libyuv/convert.h>
#include <libyuv/scale.h>
#include <libyuv/scale_argb.h>
}

#define TEXTURE_FBO_NUM 4
#define VIDEO_W 720
#define VIDEO_H 1280

class VideoFboRender : public BaseVideoRender{
private:
    GLuint textures[TEXTURE_FBO_NUM],fboTextureId,testRgbaTextureId,lutTextureId,logoTextureId;
    GLuint filterTextures[TEXTURE_FBO_NUM];
    GLuint vboIds[4],fboId;
    GLuint vaoIds[2];
    GLuint pboIds[2];
    int currentPboId= 0;
    int nextPboId= 0;
    std::mutex renderMutex;
    std::condition_variable renderCondition;
    int count = 0;
    bool renderIsFinish = false;
    int renderWidth,renderHeight,u_offset;
    float x_test_offset;


public:
    NativeOpenGLImage nativeOpenGlImage;
     void Init();
     void DrawFrame();
     void Destroy();
     void OnSurfaceChanged(int windowWidth,int windowHeight);
     void copyImage(NativeOpenGLImage *openGlImage);
     ~VideoFboRender();

    VideoFboRender(){
        fboId = fboTextureId = lutTextureId = logoTextureId  = 0;
        u_offset = 0;
        x_test_offset = 0;
    }

    void drawNormalImage();

    void readImagePixel();

    void createFbo();

    void createPbo();

    void drawFboTexture();

    void readImagePixelByPbo();

    void drawLogo(GLuint currentVaoId);
};

#endif //DEMOFFMPEG_FBORENDER_H

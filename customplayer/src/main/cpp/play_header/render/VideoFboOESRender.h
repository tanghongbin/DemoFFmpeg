//
// Created by Admin on 2021/5/13.
//


/***
 * oes扩展纹理渲染
 */
#ifndef DEMOFFMPEG_OESFBORENDER_H
#define DEMOFFMPEG_OESFBORENDER_H

#include "BaseRender.h"
#define TEXTURE_FBO_NUM 4
#define VIDEO_W 720
#define VIDEO_H 1280

class VideoFboOESRender : public BaseVideoRender{
private:
    GLuint textures[TEXTURE_FBO_NUM],fboTextureId,testRgbaTextureId,lutTextureId,logoTextureId,extTexture;
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
    float* oesMatrix;


public:
     void Init();
     void DrawFrame();
     void Destroy();
     void OnSurfaceChanged(int windowWidth,int windowHeight);
     void copyImage(NativeOpenGLImage *openGlImage);
     ~VideoFboOESRender();

    VideoFboOESRender(){
        oesMatrix = 0;
        fboId = fboTextureId = lutTextureId = logoTextureId  = 0;
        u_offset = 0;
        x_test_offset = 0;
    }

    void drawNormalImage();

    void UpdateOESMartix(float *pDouble);

    void readImagePixel();

    void createFbo();

    void createPbo();

    void drawFboTexture();

    void readImagePixelByPbo();

    void drawLogo(GLuint currentVaoId);
};

#endif //DEMOFFMPEG_FBORENDER_H

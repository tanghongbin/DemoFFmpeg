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
    GLuint fboYuv420TextureId,extTexture,fboRgbaTextureId,fboRgbaId;
    GLuint vboIds[4],fboYuv420Id;
    GLuint vaoIds[2];
    std::mutex renderMutex;
    int count = 0;
    int renderWidth,renderHeight;
    float* oesMatrix;
    Shader* rgbToYuvShader;
    Shader* rgbShader;

public:
     void Init();
     void DrawFrame();
     void Destroy();
     void OnSurfaceChanged(int windowWidth,int windowHeight);
     ~VideoFboOESRender();

    VideoFboOESRender(){
        oesMatrix = 0;
        rgbToYuvShader = rgbShader = 0;
        fboYuv420Id = fboRgbaTextureId = fboRgbaId = 0;
    }

    void copyImage(NativeOpenGLImage *openGlImage){}

    void drawNormalImage();

    void drawNormalRGBAImage();

    void UpdateOESMartix(float *pDouble);

    /***
     * 读取yuv 像素
     */
    void readYuvImagePixel();

    void readRgbaImagePixel();

    /***
     * yuv420p 720x1280的fbo
     */
    void createYuvFbo();

    /***
     * rgba 720x1280的fbo
     */
    void createRgbaFbo();

    void drawFboTexture();

    void drawYuv420pTexture();
};

#endif //DEMOFFMPEG_FBORENDER_H

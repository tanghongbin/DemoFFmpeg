//
// Created by Admin on 2021/5/13.
//

#ifndef DEMOFFMPEG_FBORENDER_H
#define DEMOFFMPEG_FBORENDER_H

#include "BaseRender.h"
#define TEXTURE_FBO_NUM 4

class VideoFboRender : public BaseVideoRender{
private:
    GLuint textures[TEXTURE_FBO_NUM],fboTextureId;
    GLuint vboIds[4],fboId;
    GLuint vaoIds[2];
    GLuint pboIds[2];
    int currentPboIndex = 0;
    int nextPboIndex = 0;
    std::mutex renderMutex;
    std::condition_variable renderCondition;
    int count = 0;
    bool renderIsFinish = false;
    int renderWidth,renderHeight;


public:
    NativeOpenGLImage nativeOpenGlImage;
     void Init();
     void DrawFrame() ;
     void Destroy();
     void OnSurfaceChanged(int windowWidth,int windowHeight);
     void copyImage(NativeOpenGLImage *openGlImage);
     ~VideoFboRender();

    VideoFboRender(){
        fboId = fboTextureId = 0;
    }

    void drawNormalImage(bool isFboRender);

    void readImagePixel();

    void createFbo();


    void createPbo();

    void readImagePixelHardBuffer();
};

#endif //DEMOFFMPEG_FBORENDER_H

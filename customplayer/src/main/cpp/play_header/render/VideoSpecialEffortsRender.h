//
// Created by Admin on 2021/5/13.
//

#ifndef DEMOFFMPEG_SPECIALEFFORTSRENDER_H
#define DEMOFFMPEG_SPECIALEFFORTSRENDER_H

#include <utils/Callback.h>
#include "BaseRender.h"
#define TEXTURE_NUM 4

class VideoSpecialEffortsRender : public BaseVideoRender{
private:
    GLuint textures[TEXTURE_NUM];
    GLuint vboIds[4];
    GLuint vaoIds[2];
    std::mutex renderMutex;
    std::condition_variable renderCondition;
    int count = 0;
    bool renderIsFinish = false;
    int widthOffset,heightOffset;
    OnReadPixelListener* onReadPixelListener;
    NativeOpenGLImage nativeOpenGlImage;
    NativeOpenGLImage specialEffortsImage;
    NativeOpenGLImage readResultImg;

    void drawImage(NativeOpenGLImage *targetImg, float d);

    void drawWaterMask();

public:
    int renderWidth,renderHeight;
    VideoSpecialEffortsRender(){
        onReadPixelListener = 0;
    }
    void Init();
    void DrawFrame();
    void Destroy();
    void OnRenderSizeChanged(int windowWidth,int windowHeight,int renderW,int renderH);
    void copyImage(NativeOpenGLImage *openGlImage);
    void copySpecialEffortsImage(NativeOpenGLImage *special);
    void destroySpecialEffortsImage();
    void setReadResultListener(OnReadPixelListener* listener){
        this->onReadPixelListener = listener;
    }
    ~VideoSpecialEffortsRender();


    glm::mat4 createMvp(bool b);
};

#endif //DEMOFFMPEG_SPECIALEFFORTSRENDER_H

//
// Created by Admin on 2021/5/13.
//

#ifndef DEMOFFMPEG_VIDEORENDER_H
#define DEMOFFMPEG_VIDEORENDER_H

#include "BaseRender.h"
#define TEXTURE_NUM 4

class VideoRender : public BaseRender{
private:
    GLuint textures[TEXTURE_NUM];
    GLuint vboIds[4];
    GLuint vaoIds[2];
    std::mutex renderMutex;
    std::condition_variable renderCondition;
    int count = 0;
    bool renderIsFinish = false;

public:
    NativeOpenGLImage nativeOpenGlImage;
     void Init();
     void DrawFrame() ;
     void Destroy();
     void copyImage(NativeOpenGLImage *openGlImage);
     ~VideoRender();
};

#endif //DEMOFFMPEG_VIDEORENDER_H

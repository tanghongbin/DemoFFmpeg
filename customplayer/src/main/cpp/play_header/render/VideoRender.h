//
// Created by Admin on 2021/5/13.
//

#ifndef DEMOFFMPEG_VIDEORENDER_H
#define DEMOFFMPEG_VIDEORENDER_H

#include "BaseRender.h"

class VideoRender : public BaseRender{
private:
    GLuint textures[4];
    GLuint vboIds[4];
    GLuint vaoIds[2];
    std::mutex customMutex;
    std::condition_variable conditionVariable;

public:
    NativeOpenGLImage * nativeOpenGlImage;
     void Init();
     void DrawFrame() ;
     void Destroy();
     void copyImage(AVFrame *openGlImage);
};

#endif //DEMOFFMPEG_VIDEORENDER_H

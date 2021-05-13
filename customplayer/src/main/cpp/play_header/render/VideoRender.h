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

public:

     void Init();
     void DrawFrame() ;
     void Destroy() ;
};

#endif //DEMOFFMPEG_VIDEORENDER_H

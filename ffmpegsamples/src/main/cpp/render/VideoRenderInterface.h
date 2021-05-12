//
// Created by Admin on 2020/9/27.
//

#ifndef DEMOC_VIDEORENDERINTERFACE_H
#define DEMOC_VIDEORENDERINTERFACE_H

#include "../utils/ImageDef.h"

class VideoRenderInterface{
public:
    virtual void init(int width,int height) = 0;
    virtual void render_video(NativeImage* nativeImage) = 0;
    virtual void unInit() = 0;
};

#endif //DEMOC_VIDEORENDERINTERFACE_H

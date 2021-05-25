//
// Created by Admin on 2021/5/13.
//

#ifndef DEMOFFMPEG_BASERENDER_H
#define DEMOFFMPEG_BASERENDER_H

#include <model/shader.h>

class BaseRender{

protected:
    Shader* shader = 0;
public:
    BaseRender(){}
    virtual ~BaseRender(){
        if (shader){
            shader->Destroy();
            delete shader;
        }
    }
    virtual void Init() = 0;
    virtual void DrawFrame() = 0;
    virtual void Destroy() = 0;
};

#endif //DEMOFFMPEG_BASERENDER_H

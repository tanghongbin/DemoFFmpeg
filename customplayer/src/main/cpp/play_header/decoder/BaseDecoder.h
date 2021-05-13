//
// Created by Admin on 2021/5/13.
//

#ifndef DEMOFFMPEG_BASEDECODER_H
#define DEMOFFMPEG_BASEDECODER_H

#include <cstdint>

class BaseDecoder{
    virtual void Init() = 0;
    virtual void Destroy() = 0;
    virtual void DecodeLoop(uint8_t* data);

};

#endif //DEMOFFMPEG_BASEDECODER_H

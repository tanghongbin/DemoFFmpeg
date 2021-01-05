//
// Created by Admin on 2020/12/30.
//

#ifndef DEMOC_ABSENCODEAV_H
#define DEMOC_ABSENCODEAV_H


#include <cstdint>
#include <thread>

class AbsEncodeAv{

public:
    virtual bool checkIsRecording() = 0;

    virtual void consume_every_frame(uint8_t* buffer, int size, int mediaType) = 0;

};

#endif //DEMOC_ABSENCODEAV_H

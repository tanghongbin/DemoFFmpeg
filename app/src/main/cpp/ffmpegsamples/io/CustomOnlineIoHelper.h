//
// Created by Admin on 2020/12/23.
//

#ifndef DEMOC_CUSTOMONLINEIOHELPER_H
#define DEMOC_CUSTOMONLINEIOHELPER_H

#define BUFFER_SIZE 4096 * 500

extern "C"{
#include "../../include/libavformat/avio.h"
#include <libavformat/avio.h>
};

class CustomOnlineIoHelper {

private:
    static int readData(void *opaque, uint8_t *buf, int buf_size);

public:
    AVIOContext* createIoContext();

    void customDestroy();

};


#endif //DEMOC_CUSTOMONLINEIOHELPER_H

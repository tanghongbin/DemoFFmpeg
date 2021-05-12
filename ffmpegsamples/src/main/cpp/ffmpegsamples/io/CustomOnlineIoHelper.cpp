//
// Created by Admin on 2020/12/23.
//


#include "CustomOnlineIoHelper.h"

int CustomOnlineIoHelper::readData(void *opaque, uint8_t *buf, int buf_size) {

}

AVIOContext* CustomOnlineIoHelper::createIoContext() {
    uint8_t * buffer = static_cast<uint8_t *>(av_malloc(sizeof(uint8_t) * BUFFER_SIZE));
    AVIOContext *pb = avio_alloc_context(buffer, BUFFER_SIZE, 0, NULL, readData, NULL, NULL);

}

void CustomOnlineIoHelper::customDestroy() {

}
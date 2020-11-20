//
// Created by Admin on 2020/11/20.
//

#ifndef DEMOC_SINGLENODELIST_H
#define DEMOC_SINGLENODELIST_H

#include <mutex>
#include "../../../../../../../android_sdk/android_sdk/sdk/ndk-bundle/toolchains/llvm/prebuilt/windows-x86_64/sysroot/usr/include/c++/v1/cstdint"
#include "../include/libavcodec/avcodec.h"

/**
 * 单向链表，暂时是线程不安全的,不支持模版
 */

typedef struct CustomNode_ {
    const char *point;
    struct CustomNode_ *next;

    CustomNode_ (){
        point = nullptr;
        next = nullptr;
    }

} CustomNode;

class SingleNodeList {
private:
    CustomNode *firstNode;
    CustomNode *curNode;
    std::mutex mutex;

public:

    SingleNodeList();

    ~SingleNodeList();

    void pushLast(CustomNode *node);

    CustomNode *popFirst();

};


#endif //DEMOC_SINGLENODELIST_H

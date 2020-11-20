//
// Created by Admin on 2020/11/20.
//

#include "SingleNodeList.h"
#include "utils.h"

SingleNodeList::SingleNodeList() {
    firstNode = nullptr;
    curNode = nullptr;
}

SingleNodeList::~SingleNodeList() {
    LOGCATE("i'm free firstNode:%p", firstNode);
    while (firstNode) {
        LOGCATE("i'm start free:%p", firstNode);
        CustomNode *next = firstNode->next;
        firstNode->next = nullptr;
        delete firstNode;
        firstNode = next;
    }
    LOGCATE("i'm free over firstNode:%p", firstNode);
}


void SingleNodeList::pushLast(CustomNode *node) {
    // 放入第一个
//    LOGCATE("push node:%p str:%s",node->next,node->point);

    std::unique_lock<std::mutex> lock(mutex);
    if (!firstNode) {
        firstNode = node;
    }
    if (!curNode) {
        curNode = node;
    } else {
        curNode->next = node;
        curNode = node;
    }
}


CustomNode *SingleNodeList::popFirst() {
    std::unique_lock<std::mutex> lock(mutex);
    if (firstNode) {
        CustomNode *tempFirst = firstNode;
        firstNode = tempFirst->next;
        tempFirst->next = nullptr;
        return tempFirst;
    }
    return nullptr;
}


//
// Created by Admin on 2020/11/20.
//

#include "SingleNodeList.h"
#include "utils.h"

template<class T>
SingleNodeList<T>::SingleNodeList() {
    firstNode = nullptr;
    curNode = nullptr;
}

template<class T>
SingleNodeList<T>::~SingleNodeList() {
    LOGCATE("i'm free firstNode:%p", firstNode);
    while (firstNode) {
        LOGCATE("i'm start free:%p", firstNode);
        CustomNode<T> *next = firstNode->next;
        firstNode->next = nullptr;
        delete firstNode;
        firstNode = next;
    }
    LOGCATE("i'm free over firstNode:%p", firstNode);
}


template<class T>
void SingleNodeList<T>::pushLast(T node) {
    // 放入第一个
//    LOGCATE("push node:%p str:%s",node->next,node->point);

    std::unique_lock<std::mutex> lock(mutex);
    CustomNode<T>* newNode = new CustomNode<T>(node);
    if (!firstNode) {
        firstNode = newNode;
    }
    if (!curNode) {
        curNode = newNode;
    } else {
        curNode->next = newNode;
        curNode = newNode;
    }
}


template<class T>
T SingleNodeList<T>::popFirst() {
    std::unique_lock<std::mutex> lock(mutex);
    if (firstNode) {
        CustomNode<T> *tempFirst = firstNode;
        firstNode = tempFirst->next;
        tempFirst->next = nullptr;
        return tempFirst->point;
    }
    return nullptr;
}


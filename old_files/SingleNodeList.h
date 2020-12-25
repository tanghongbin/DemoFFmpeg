//
// Created by Admin on 2020/11/20.
//

#ifndef DEMOC_SINGLENODELIST_H
#define DEMOC_SINGLENODELIST_H

#include <mutex>
#include "cstdint"
#include "libavcodec/avcodec.h"

/**
 * 单向链表，暂时是线程不安全的,不支持模版
 */

template <class T>
class CustomNode {
public:
    T point;
    CustomNode<T> *next;
    CustomNode(T node){
        point = node;
    }

    ~CustomNode (){
        point = nullptr;
        next = nullptr;
    }
};

template <class T>
class SingleNodeList {
private:
    CustomNode<T> *firstNode;
    CustomNode<T> *curNode;
    std::mutex mutex;

public:

    SingleNodeList<T>();

    ~SingleNodeList<T>();

     void pushLast(T node);

     T popFirst();

};


#endif //DEMOC_SINGLENODELIST_H

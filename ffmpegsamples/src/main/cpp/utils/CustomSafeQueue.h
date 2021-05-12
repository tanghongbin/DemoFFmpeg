//
// Created by Admin on 2020/11/20.
//

#ifndef DEMOC_CUSTOMSAFEQUEUE_H
#define DEMOC_CUSTOMSAFEQUEUE_H


//#include <queue>

//using namespace std;
#include <stdlib.h>
#include <mutex>

#include <queue>

/**
 * 线程安全，并且为空会阻塞
 */

template<typename T>
class CustomSafeQueue {
private:
    std::queue<T> mQueue;
    std::mutex tex;
    std::condition_variable conditionVariable;

public:

    CustomSafeQueue<T>() {

    }

    ~CustomSafeQueue<T>() {
        // todo 自己清除
    }

    void pushLast(T node) {
        // 放入第一个
        std::unique_lock<std::mutex> lockGuard(tex);
        mQueue.push(node);
        conditionVariable.notify_one();
    }

    T popFirst() {
        std::unique_lock<std::mutex> uniqueLock(tex);
        if (mQueue.size() == 0) {
            conditionVariable.wait(uniqueLock);
        }
        T bean = mQueue.front();
        mQueue.pop();
        return bean;
    }

    T removeFirst() {
        T bean = mQueue.front();
        mQueue.pop();
        return bean;
    }

    int size(){
        return mQueue.size();
    }
};


#endif //DEMOC_CUSTOMSAFEQUEUE_H

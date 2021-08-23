//
// Created by Admin on 2020/11/20.
//

#ifndef DEMOC_CUSTOMSAFEQUEUE_H
#define DEMOC_CUSTOMSAFEQUEUE_H


//#include <queue>

//using namespace std;
#include <stdlib.h>
#include <mutex>
#include <utils/utils.h>

#include <queue>

/**
 * 线程安全，并且为空会阻塞
 */

template<typename T>
class CustomSafeBlockQueue {
private:
    std::queue<T> mQueue;
    std::mutex tex;
    std::condition_variable conditionVariable;

public:

    CustomSafeBlockQueue<T>() {

    }

    ~CustomSafeBlockQueue<T>() {
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
        std::unique_lock<std::mutex> uniqueLock(tex);
        return mQueue.size();
    }
};

template<typename T>
class CustomSafeQueue {
private:
    std::queue<T> mQueue;
    std::mutex tex;
    int inCount,outCount;
    int maxSize;

public:
    
    void setMax(int max){
        maxSize = max;
        LOGCATE("log max Size:%d %d",max,maxSize);
    }
    
    CustomSafeQueue<T>() {
        maxSize = INT_MAX;
    }

    ~CustomSafeQueue<T>() {
        LOGCATE("total enter count:%d  outcount:%d",inCount,outCount);
        inCount = outCount = 0;
    }

    T pushLast(T node) {
        T last = nullptr;
        // 放入第一个
        std::unique_lock<std::mutex> lockGuard(tex);
        mQueue.push(node);
        if (mQueue.size() > maxSize){
            last = mQueue.front();
            mQueue.pop();
        }
        inCount++;
//        LOGCATE("totalSize:%d max:%d",mQueue.size(),maxSize);
        return last;
    }

    T popFirst() {
        std::unique_lock<std::mutex> uniqueLock(tex);
        if (mQueue.size() == 0) {
            return nullptr;
        }
        T bean = mQueue.front();
        mQueue.pop();
        outCount++;
        return bean;
    }

    T removeFirst() {
        T bean = mQueue.front();
        mQueue.pop();
        return bean;
    }

    int size(){
        std::unique_lock<std::mutex> lockGuard(tex);
        return mQueue.size();
    }
};


#endif //DEMOC_CUSTOMSAFEQUEUE_H

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
#include <map>
#include <iterator>

/**
 * 线程安全，并且为空会阻塞
 */

template<typename T>
class CustomSafeBlockQueue {
private:
    std::queue<T> mQueue;
    std::mutex tex;
    std::condition_variable conditionVariable;
    int maxSize;
    int inCount,outCount;


public:

    void setMax(int max){
        maxSize = max;
    }


    CustomSafeBlockQueue<T>() {
        inCount = outCount = 0;
        maxSize = INT_MAX;
    }

    ~CustomSafeBlockQueue<T>() {
    }

    T pushLast(T node) {
        // 放入第一个
        T last;
        std::unique_lock<std::mutex> lockGuard(tex);
        mQueue.push(node);
        if (mQueue.size() > maxSize){
            last = mQueue.front();
            mQueue.pop();
        }
        inCount++;
        conditionVariable.notify_all();
        return last;
    }

    T popFirst() {
        std::unique_lock<std::mutex> uniqueLock(tex);
        if (mQueue.size() == 0) {
            conditionVariable.wait(uniqueLock);
        }
        outCount++;
        T bean = mQueue.front();
        mQueue.pop();
        return bean;
    }

    /***
     * 是否到达最大容量
     * @return
     */
    bool isFull(){
        std::unique_lock<std::mutex> lockGuard(tex);
        return mQueue.size() >= maxSize;
    }

    T getFirst() {
        std::unique_lock<std::mutex> uniqueLock(tex);
        if (mQueue.size() == 0) {
            return nullptr;
        }
        outCount++;
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
    }
    
    CustomSafeQueue<T>() {
        maxSize = INT_MAX;
    }

    ~CustomSafeQueue<T>() {
        inCount = outCount = 0;
    }

    T pushLast(T node) {
        T last = 0;
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

    /***
     * 是否到达最大容量
     * @return
     */
    bool isFull(){
        std::unique_lock<std::mutex> lockGuard(tex);
        return mQueue.size() >= maxSize;
    }

    T popFirst() {
        std::unique_lock<std::mutex> uniqueLock(tex);
        if (mQueue.size() == 0) {
            return 0;
        }
        T bean = mQueue.front();
        mQueue.pop();
        outCount++;
        return bean;
    }

    T getLast() {
        std::unique_lock<std::mutex> uniqueLock(tex);
        if (mQueue.size() == 0) {
            return 0;
        }
        T bean = mQueue.back();
        outCount++;
        return bean;
    }

    int size(){
        std::unique_lock<std::mutex> lockGuard(tex);
        return mQueue.size();
    }
};

//template<typename KEY,typename VALUE>
//class CustomSafeHashMap{
//private:
//    std::map<KEY,VALUE> customMap;
//    std::mutex customMutex;
//public:
//    void put(KEY key,VALUE value){
//        std::lock_guard<std::mutex> lock(customMutex);
//        customMap.insert(std::map<KEY,VALUE>::value_type(key,value));
//    }
//    VALUE get(KEY key){
//        const std::map<KEY,VALUE>::iterator item = customMap.find(key);
//        if (item == customMap.end()) return nullptr;
//        return item.first;
//    }
//};


#endif //DEMOC_CUSTOMSAFEQUEUE_H

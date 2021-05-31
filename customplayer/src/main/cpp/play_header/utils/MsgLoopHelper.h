//
// Created by Admin on 2021/5/21.
//

#ifndef DEMOFFMPEG_MSGLOOPHELPER_H
#define DEMOFFMPEG_MSGLOOPHELPER_H

#include "Message.h"
#include <thread>
#include <cstdint>
#include <utils/CustomSafeQueue.h>

class MsgLoopHelper{
private:
    bool isLoop = true;
    CustomSafeQueue<Message*> * safeQueue;
    std::thread * msgThread;
    static MsgLoopHelper* instance;

    static void prepareMsgLoop(MsgLoopHelper* player);

    void destroyMsgLoop();

    static MsgLoopHelper* getInstance(){
        if (instance == nullptr) instance = new MsgLoopHelper;
        return instance;
    }


public:
    static void destroyInstance(){
        if (instance == nullptr) return;
        getInstance()->destroyMsgLoop();
        delete instance;
        instance = nullptr;
    }

    static void initMsgLoop();

    static void sendMsg(Message* msg);

};

#endif //DEMOFFMPEG_MSGLOOPHELPER_H

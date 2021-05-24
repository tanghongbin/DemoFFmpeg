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


public:
    static MsgLoopHelper* getInstance(){
        if (instance == nullptr) instance = new MsgLoopHelper;
        return instance;
    }
    static void destroyInstance(){
        getInstance()->destroyMsgLoop();
        delete instance;
        instance = nullptr;
    }

    void initMsgLoop();


    void sendMsg(Message* msg);

};

#endif //DEMOFFMPEG_MSGLOOPHELPER_H

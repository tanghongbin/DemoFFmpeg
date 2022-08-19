//
// Created by Admin on 2021/5/21.
//

#ifndef DEMOFFMPEG_MSGLOOPHELPER_H
#define DEMOFFMPEG_MSGLOOPHELPER_H

#include "Message.h"
#include <thread>
#include <cstdint>
#include <utils/CustomSafeBlockQueue.h>

class MsgLoopHelper{
private:
    bool isLoop;
    CustomSafeBlockQueue<Message*> safeQueue;
    std::thread * msgThread;
    static MsgLoopHelper* instance;

    static void prepareMsgLoop(MsgLoopHelper* player);

    void stopLoop();

    static MsgLoopHelper* getInstance(){
        if (instance == nullptr) instance = new MsgLoopHelper;
        return instance;
    }

    MsgLoopHelper(){
        instance = 0;
        msgThread = 0;
        isLoop = true;
    }


public:
    static void destroyInstance(){
        if (instance == nullptr) return;
        getInstance()->stopLoop();
        delete instance;
        instance = nullptr;
    }

    static void initMsgLoop();

    static void sendMsg(Message* msg);

};

#endif //DEMOFFMPEG_MSGLOOPHELPER_H

//
// Created by Admin on 2021/5/21.
//

#include <utils/MsgLoopHelper.h>
#include <utils/JavaVmManager.h>
#include <unistd.h>

MsgLoopHelper* MsgLoopHelper::instance = nullptr;

void MsgLoopHelper::prepareMsgLoop(MsgLoopHelper* player){
    LOGCATE("loop has enter %p",player);
    for (;;) {

        if (!player->isLoop) break;
        Message*  message = player->safeQueue.popFirst();
        if (message == nullptr) continue;
        bool isAttach;
        JNIEnv *env = JavaVmManager::GetEnv(&isAttach);
        jobject javaPlayerIns = JavaVmManager::getObjInstance();
//        LOGCATE("check instance  instance:%p",);
        jclass jclass1 = env->GetObjectClass(JavaVmManager::getObjInstance());
        jmethodID methodId = env->GetMethodID(jclass1,"receivePlayerMsgFromJni","(IIILjava/lang/String;)V");
        if (methodId){
            jstring result = env->NewStringUTF(message->msg.c_str());
//            LOGCATE("current thread :%d",GetCurrent());
            env->CallVoidMethod(javaPlayerIns,methodId,message->type,message->arg1,message->arg2,result);
//            env->CallStaticVoidMethod()
        }
        if (isAttach){
            JavaVmManager::detachCurrentThread();
        }
        message->recycle();
    }
    LOGCATE("loop msg has end");
}
void MsgLoopHelper::initMsgLoop(){
    if (getInstance()->msgThread == nullptr) getInstance()->msgThread = new std::thread(prepareMsgLoop,getInstance());
}

void MsgLoopHelper::sendMsg(Message *msg){
    getInstance()->safeQueue.pushLast(msg);
}

void MsgLoopHelper::destroyMsgLoop(){
    isLoop = false;
    safeQueue.pushLast(nullptr);
    if (msgThread) {
        msgThread->join();
        delete msgThread;
    }
    msgThread = nullptr;
    if (instance) delete instance;
    instance = nullptr;
}
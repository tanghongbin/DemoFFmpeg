//
// Created by Admin on 2021/4/29.
//

#ifndef DEMOFFMPEG_CALLBACK_H
#define DEMOFFMPEG_CALLBACK_H

#include <jni.h>

class MsgCallback{
public:
    virtual void call(JNIEnv* jni,jobject obj,jmethodID methodId) {
        LOGCATE("call has called father");
    }
};

#endif //DEMOFFMPEG_CALLBACK_H

//
// Created by Admin on 2020/10/19.
//

#ifndef DEMOC_JAVAVMMANAGER_H
#define DEMOC_JAVAVMMANAGER_H


#include <jni.h>

class JavaVmManager {
private:
    static JavaVM* javaVm;

public:
    static void initVm(JNIEnv *jniEnv);

    static JNIEnv *GetEnv(bool *attach);

    static void detachCurrentThread();
};


#endif //DEMOC_JAVAVMMANAGER_H

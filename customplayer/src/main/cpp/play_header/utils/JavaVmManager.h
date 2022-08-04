//
// Created by Admin on 2020/10/19.
//

#ifndef DEMOC_JAVAVMMANAGER_H
#define DEMOC_JAVAVMMANAGER_H


#include <jni.h>

class JavaVmManager {
private:
    static JavaVM* javaVm;
    static jobject instance;

public:
    static void initVm(JNIEnv *jniEnv);

    static JNIEnv *GetEnv(bool *attach);

    static void detachCurrentThread();
    
    static void setInstance(JNIEnv* env,jobject paras){
        if (instance == nullptr) instance = env->NewGlobalRef(paras);
    }
    static jobject getObjInstance(){
        return instance;
    }
    static void destroyInstance(JNIEnv *env) {
        if (instance == nullptr) return;
        env->DeleteGlobalRef(instance);
        instance = nullptr;
    }
    static JavaVM* getJavaVM(){
        return javaVm;
    }
    
};


#endif //DEMOC_JAVAVMMANAGER_H

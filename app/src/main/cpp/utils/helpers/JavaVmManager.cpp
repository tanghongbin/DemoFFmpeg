//
// Created by Admin on 2020/10/19.
//

#include <utils.h>
#include "JavaVmManager.h"

JavaVM* JavaVmManager::javaVm = nullptr;

void JavaVmManager::detachCurrentThread() {
    javaVm->DetachCurrentThread();
    LOGCATE("detach thread success");
}

void JavaVmManager::initVm(JNIEnv *jniEnv)  {
    jniEnv->GetJavaVM(&javaVm);
    LOGCATE("init jvm success");
}

JNIEnv *JavaVmManager::GetEnv(bool *attach) {
    JNIEnv *jniEnv;
    int status = javaVm->GetEnv(reinterpret_cast<void **>(&jniEnv), JNI_VERSION_1_6);
    if (status != JNI_OK) {
        status = javaVm->AttachCurrentThread(&jniEnv, nullptr);
        if (status != JNI_OK) {
            return nullptr;
        }
        *attach = true;
    }
    return jniEnv;
}
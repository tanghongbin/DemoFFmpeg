//
// Created by Admin on 2020/10/19.
//

#include "../../play_header/utils/utils.h"
#include "../../play_header/utils/JavaVmManager.h"

JavaVM* JavaVmManager::javaVm = nullptr;
jobject JavaVmManager::instance = nullptr;

void JavaVmManager::detachCurrentThread() {
    javaVm->DetachCurrentThread();
//    LOGCATE("detach thread success");
}

void JavaVmManager::initVm(JNIEnv *jniEnv)  {
    if (javaVm == nullptr) jniEnv->GetJavaVM(&javaVm);
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
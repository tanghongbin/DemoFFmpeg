//
// Created by Admin on 2020/10/10.
//

#include <CustomGLUtils.h>
#include "PlayMp4Instance.h"

JavaVM *PlayMp4Instance::mJavaVm = nullptr;
jobject PlayMp4Instance::mNativeRender = nullptr;


void PlayMp4Instance::init(const char *url, JNIEnv *jniEnv, jobject nativeRender, jobject surface) {
    LOGCATE("init has started");
    strcpy(mPlayUrl, url);
    LOGCATE("init has reached address playUrl:%s", mPlayUrl);
    jniEnv->GetJavaVM(&mJavaVm);
    LOGCATE("init has reached vm success");
    mNativeRender = jniEnv->NewGlobalRef(nativeRender);
    mSurfaceInstance = jniEnv->NewGlobalRef(surface);
    LOGCATE("prepare created short thread");
    audio = new thread(createThreadForPlay, mNativeRender, url, mSurfaceInstance, 1);
    video = new thread(createThreadForPlay, mNativeRender, url, mSurfaceInstance, 2);
    LOGCATE("thread has started");
}

void PlayMp4Instance::unInit() {
    bool isAttach = false;
    JNIEnv *jniEnv = GetEnv(&isAttach);
    jniEnv->DeleteGlobalRef(mNativeRender);
    jniEnv->DeleteGlobalRef(mSurfaceInstance);
    if (isAttach) mJavaVm->DetachCurrentThread();
    if (audio) {
        delete audio;
        audio = nullptr;
    }
    if (video) {
        delete video;
        video = nullptr;
    }
}


JNIEnv *PlayMp4Instance::GetEnv(bool *attach) {
    JNIEnv *jniEnv;
    int status = mJavaVm->GetEnv(reinterpret_cast<void **>(&jniEnv), JNI_VERSION_1_6);
    if (status != JNI_OK) {
        status = mJavaVm->AttachCurrentThread(&jniEnv, nullptr);
        if (status != JNI_OK) {
            return nullptr;
        }
        *attach = true;
    }
    return jniEnv;
}

/**
 * 发消息给java层
 * @param type
 */
void PlayMp4Instance::sendMsg(int type) {
    bool isAttach = false;
    JNIEnv *jniEnv = GetEnv(&isAttach);
    jmethodID id = jniEnv->GetMethodID(jniEnv->GetObjectClass(mNativeRender),
                                       MSG_CALLBACK_FUNCTION_NAME, "(I)V");
    jniEnv->CallVoidMethod(mNativeRender, id, type);
    LOGCATE("sendmsg success type:%d", type);
    if (isAttach) mJavaVm->DetachCurrentThread();
}


void PlayMp4Instance::detachCurrentThread() {
    mJavaVm->DetachCurrentThread();
}
//
// Created by Admin on 2020/10/10.
//

#ifndef DEMOC_PLAYMP4INSTANCE_H
#define DEMOC_PLAYMP4INSTANCE_H

#include <thread>
#include <jni.h>

#define MSG_TYPE_ERROR  0
#define MSG_TYPE_READY  1
#define MSG_CALLBACK_FUNCTION_NAME "nativeMsgCallback"

using namespace std;

class PlayMp4Instance {

private:
    char mPlayUrl[2048] = {0};
    thread *audio;
    thread *video;
    _jobject surface;
    static JavaVM *mJavaVm;
    static jobject mNativeRender;
    jobject mSurfaceInstance;

public:

    void init(const char *url, JNIEnv *jniEnv, jobject nativeRender, jobject surface);

    void unInit();

    static JNIEnv *GetEnv(bool *attach);

    static void sendMsg(int i);

    static void detachCurrentThread();
};


#endif //DEMOC_PLAYMP4INSTANCE_H

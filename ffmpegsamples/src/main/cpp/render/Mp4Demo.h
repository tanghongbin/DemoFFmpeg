//
// Created by Admin on 2020/10/10.
//

#ifndef DEMOC_PLAYMP4INSTANCE_H
#define DEMOC_PLAYMP4INSTANCE_H

#include <thread>
#include <jni.h>
#include "BaseRender.h"

#define MSG_TYPE_ERROR  0
#define MSG_TYPE_READY  1
#define MSG_CALLBACK_FUNCTION_NAME "nativeMsgCallback"

using namespace std;

enum PLAY_STATUS{
    RESUME,
    PAUSE
};

class Mp4Demo {

private:
    char mPlayUrl[2048] = {0};
    thread *audio;
    thread *video;
    _jobject surface;
    static jobject mNativeRender;
    jobject mSurfaceInstance;
    PLAY_STATUS mPlayStatus = RESUME;
    mutex mMutex;
    condition_variable signal;


public:

    static long totalDuration;

    void init(const char *url, JNIEnv *jniEnv, jobject nativeRender, jobject surface,int viedoType);

    void unInit();

    void seekPosition(int position);

    static void createThreadForPlay(Mp4Demo* selefInstance, _jobject *instance, const char *localUrl, _jobject *pJobject, jint type);

    void
    decodeLoop(Mp4Demo *pInstance, AVFormatContext *mFormatContext,
               AVCodecContext *decode_context,
               AVPacket *packet, AVFrame *frame, BaseRender* baseRender,
               jobject pJobject, int m_StreamIndex);

    void resume();

    void pauseManual();
};


#endif //DEMOC_PLAYMP4INSTANCE_H

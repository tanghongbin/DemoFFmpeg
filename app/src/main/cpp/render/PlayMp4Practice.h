//
// Created by Admin on 2020/10/19.
//

#ifndef DEMOC_PLAYMP4PRACTICE_H
#define DEMOC_PLAYMP4PRACTICE_H

#include <thread>
#include <jni.h>
#include "BaseRender.h"

extern "C"{
#include <libavformat/avformat.h>
};

using namespace std;

class PlayMp4Practice {
private:
    thread* mThread;


public:

    JavaVM *mVm;
    char mUrl[1024];
    jobject mRenderInstance;
    jobject mSurface;

    void init(const char* url,JNIEnv* jniEnv,jobject renderInstance,jobject surface,int type);

    static void
    createPlayProcess(PlayMp4Practice *pPractice, jobject pJobject, jobject pJobject1, int i);

    bool checkNegativeReturn(int ret, const char string[18]);

    BaseRender *createRender(int type);

    void decodeLoop(AVPacket *pPacket, AVFrame *pFrame, BaseRender *pRender, jobject pJobject,
                    jobject pJobject1, AVCodecContext *codecContext, AVFormatContext *pContext,
                    int i);
};


#endif //DEMOC_PLAYMP4PRACTICE_H

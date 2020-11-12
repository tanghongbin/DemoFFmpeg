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


#define ENABLE_AV_FILTER true
using namespace std;

class PlayMp4Practice {
private:
    thread* mThread;
    bool isLoop = true;


public:

    JavaVM *mVm;
    char mUrl[1024];
    jobject mRenderInstance;
    jobject mSurface;


    void init(const char* url,JNIEnv* jniEnv,jobject renderInstance,jobject surface,int type);

    static void
    createPlayProcess(PlayMp4Practice *pPractice, jobject pJobject, jobject pJobject1, int i);

    BaseRender *createRender(int type);

    void decodeLoop(AVPacket *pPacket, AVFrame *pFrame, BaseRender *pRender, jobject pJobject,
                    jobject pJobject1, AVCodecContext *codecContext, AVFormatContext *pContext,
                    int i);

    /**
     * 带滤镜的解析
     * @param pPacket
     * @param pFrame
     * @param pRender
     * @param pJobject
     * @param pJobject1
     * @param codecContext
     * @param pContext
     * @param i
     */
    void decodeLoopWithFilter(AVPacket *pPacket, AVFrame *pFrame, BaseRender *pRender, jobject pJobject,
                    jobject pJobject1, AVCodecContext *codecContext, AVFormatContext *pContext,
                    int i);

    void stopPlay();
};


#endif //DEMOC_PLAYMP4PRACTICE_H

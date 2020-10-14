//
// Created by Admin on 2020/9/16.
//

#ifndef DEMOC_BASERENDER_H
#define DEMOC_BASERENDER_H

extern "C" {
#include "../include/libavcodec/avcodec.h"
#include "../include/libavutil/frame.h"
#include "../include/libavformat/avformat.h"
};


#include "../../../../../../../android_sdk/android_sdk/sdk/ndk-bundle/toolchains/llvm/prebuilt/windows-x86_64/sysroot/usr/include/jni.h"
#include "VideoRenderInterface.h"

class BaseRender {

private:
    static long mSeekAudioPosition;
    static long mSeekVideoPosition;

public:

    VideoRenderInterface *mRenderInterface;

    virtual void init(AVCodecContext *pContext, _jobject *instance, _jobject *pJobject) = 0;

    virtual void draw_frame(AVCodecContext *pContext, AVFrame *pFrame, _jobject *pJobject) = 0;

    virtual void unInit() = 0;

    virtual void eachPacket(AVPacket *packet, AVCodecContext *pContext) = 0;

    static void setupSeekPosition(int position, int type) {
        if (type == 1) {
            mSeekAudioPosition = position;
        } else {
            mSeekVideoPosition = position;
        }
        LOGCATE("current type:%d seekPosition:%d",type,position);
    }

    void
    seekPositionFunc(int streamIndex, AVFormatContext *pFormatContext, AVCodecContext *pContext) {
        long *targetPostion = streamIndex == 1 ? &mSeekAudioPosition : &mSeekVideoPosition;
        if (*targetPostion != -1) {
            float target = *targetPostion * 1.0f;
            LOGCATE("look at streamIndex:%d targetPosition:%d", streamIndex,*targetPostion);
            int64_t seek_target = static_cast<int64_t>((target) * 1000000);//微秒
            int seekResult = avformat_seek_file(pFormatContext, -1, INT64_MIN, seek_target,
                                                INT64_MAX, 0);
            if (seekResult == 0) {
                avcodec_flush_buffers(pContext);
                *targetPostion = -1;
            }
            LOGCATE("after setting ,look value-audio:%d  video-value:%d", mSeekAudioPosition,
                    mSeekVideoPosition);
        }

    }

};

#endif //DEMOC_BASERENDER_H

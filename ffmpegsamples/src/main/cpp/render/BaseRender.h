//
// Created by Admin on 2020/9/16.
//

#ifndef DEMOC_BASERENDER_H
#define DEMOC_BASERENDER_H

extern "C" {
#include "../include/libavcodec/avcodec.h"
#include "../include/libavutil/frame.h"
#include "../include/libavformat/avformat.h"
}

#include <jni.h>
#include "VideoRenderInterface.h"

#define MAX_PROGRESS_SEEK_BAR 100

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

    virtual void onDecodeReady(AVFormatContext * formatContext,AVCodecContext *codecContext,jobject instance){}

    static void setupSeekPosition(int position, int type) {
        if (type == 1) {
            mSeekAudioPosition = position;
        } else {
            mSeekVideoPosition = position;
        }
        LOGCATE("current type:%d seekPosition:%d",type,position);
    }

    virtual void seekPositionFunc(int streamIndex, AVFormatContext *pFormatContext, AVCodecContext *pContext) {
        long *targetPostion = streamIndex == 1 ? &mSeekAudioPosition : &mSeekVideoPosition;
//        LOGCATE("log position:%ld",*targetPostion);
        if (*targetPostion != -1) {
            LOGCATE("look at streamIndex:%d targetPosition:%ld", streamIndex,*targetPostion);
            int64_t seek_target = *targetPostion * AV_TIME_BASE;//微秒
            LOGCATE("log duration:%lld target:%lld",pFormatContext->duration,seek_target);
            int seekResult = avformat_seek_file(pFormatContext, -1, INT64_MIN, seek_target,
                                                INT64_MAX, 0);
            if (seekResult == 0) {
                avcodec_flush_buffers(pContext);
                *targetPostion = -1;
                LOGCATE("切换位置:%lld",seek_target);
            }
            LOGCATE("after setting ,look value-audio:%d  video-value:%d", mSeekAudioPosition,
                    mSeekVideoPosition);
        }


    }

};

#endif //DEMOC_BASERENDER_H

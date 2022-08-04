//
// Created by Admin on 2021/8/23.
//

#ifndef DEMOFFMPEG_ENCODERAACANDX264_H
#define DEMOFFMPEG_ENCODERAACANDX264_H

#include <cstdlib>
#include <thread>
#include <mutex>
#include <utils/CustomSafeBlockQueue.h>
#include <rtmp/RtmpPushHelper.h>
#include "utils/CustomGLUtils.h"

using std::thread;
using std::mutex;
/***
 * fdk-aac 和 x264 编码器, 编码音频和视频
 */
class EncoderAACAndx264 {

private:
    RtmpPushHelper* rtmpPushHelper;
    thread* mEncodeThreadV;
    thread* mEncodeThreadA;
    bool isRunning;
    mutex mAvMutex;
    static void loopEncodeVideo(EncoderAACAndx264* instance);
    static void loopEncodeAudio(EncoderAACAndx264* instance);
    CustomSafeBlockQueue<NativeOpenGLImage*>mVideoQueue;
    CustomSafeBlockQueue<AudioRecordItemInfo *>mAudioQueue;

public:
    EncoderAACAndx264() {
        isRunning = true;
        mEncodeThreadV = mEncodeThreadA =  0;
        rtmpPushHelper = 0;
        mVideoQueue.setMax(3);
    }
    ~EncoderAACAndx264() {

    }

    void init();
    void destroy();

    // 1-音频，2-视频
    void putVideoData(NativeOpenGLImage* data);

    void putAudioData(uint8_t *string, jint i);
};


#endif //DEMOFFMPEG_ENCODERAACANDX264_H

//
// Created by Admin on 2021/8/23.
//

#ifndef DEMOFFMPEG_ENCODERAACANDX264_H
#define DEMOFFMPEG_ENCODERAACANDX264_H

#include <cstdlib>
#include <thread>
#include <mutex>
#include <utils/CustomSafeBlockQueue.h>
#include "utils/CustomGLUtils.h"

using std::thread;
using std::mutex;
/***
 * fdk-aac 和 x264 编码器
 */
class EncoderAACAndx264 {

private:
    thread* mEncodeThreadV;
    bool isRunning;
    mutex mAvMutex;
    static void loopEncodeVideo(EncoderAACAndx264* instance);
    CustomSafeBlockQueue<NativeOpenGLImage*>mVideoQueue;
    CustomSafeBlockQueue<AudioRecordItemInfo *>mAudioQueue;

public:
    EncoderAACAndx264() {
        isRunning = true;
        mEncodeThreadV = 0;
        mVideoQueue.setMax(3);
    }
    ~EncoderAACAndx264() {

    }

    void init();
    void destroy();

    // 1-音频，2-视频
    void encodeData(int type,uint8_t* data);


    void addH264Header(uint8_t *string, uint8_t *string1, int i, int i1);

    void addH264Body(uint8_t *string, int i);
};


#endif //DEMOFFMPEG_ENCODERAACANDX264_H

//
// Created by Admin on 2021/7/16.
//

#ifndef DEMOFFMPEG_MEDIACODECVIDEO_H
#define DEMOFFMPEG_MEDIACODECVIDEO_H

#include <cstdint>
#include <thread>
#include <encoder/AbsMediaMuxer.h>
#include <utils/utils.h>
#include "AudioConfiguration.h"
#include <utils/CustomSafeBlockQueue.h>

/***
 *
 * 视频硬编码
 *
 */

class MediaCodecVideo {
private:
    double speed;
    std::thread* encodeThread;
    bool isRunning;
    AMediaCodec *mMediaCodec;
    AMediaFormat *mMediaFormat;
    CustomSafeQueue<NativeOpenGLImage*> videoQueue;
    OutputFmtChangedListener outputFmtChangedListener;
    OutputDataListener outputDataListener;
    int64_t startNanoTime,totalTime;

    AMediaCodec * createVideoMediaCodec();

public:
    MediaCodecVideo(){
        isRunning = false;
        startNanoTime = totalTime = 0LL;
        mMediaCodec = 0;
        mMediaFormat = 0;
        encodeThread = 0;
        videoQueue.setMax(3);
        speed = 1.0;
    }
    void setSpeed(double speed);
    void setOutputDataListener(OutputFmtChangedListener changedListener,OutputDataListener dataListener);
    void startEncode();
    void destroy();
    void putData(NativeOpenGLImage* image);
    static void loopEncode(MediaCodecVideo* codecAudio);
    int64_t getTotalTime();
};

#endif //DEMOFFMPEG_MEDIACODECVIDEO_H

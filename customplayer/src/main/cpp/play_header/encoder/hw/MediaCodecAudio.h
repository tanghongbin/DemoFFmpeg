//
// Created by Admin on 2021/7/16.
//

#ifndef DEMOFFMPEG_MEDIACODECAUDIO_H
#define DEMOFFMPEG_MEDIACODECAUDIO_H

#include <cstdint>
#include <thread>
#include <encoder/AbsMediaMuxer.h>
#include <utils/utils.h>
#include "AudioConfiguration.h"
#include <utils/CustomSafeBlockQueue.h>

/***
 * 音频硬编码
 *
 */

class MediaCodecAudio {
private:
    std::thread* encodeThread;
    bool isRunning;
    AMediaCodec *mMediaCodec;
    AMediaFormat *mMediaFormat;
    CustomSafeQueue<AudioRecordItemInfo*> audioQueue;
    OutputFmtChangedListener outputFmtChangedListener;
    OutputDataListener outputDataListener;
    int64_t mStartTime;
    AMediaCodec * createAudioMediaCodec();
    static void loopEncode(MediaCodecAudio* codecAudio);

public:
    MediaCodecAudio(){
        isRunning = false;
        mMediaCodec = 0;
        mMediaFormat = 0;
        encodeThread = 0;
        mStartTime = -1LL;
    }
    void setOutputDataListener(OutputFmtChangedListener changedListener,OutputDataListener dataListener);
    void startEncode();
    void destroy();
    void putData(uint8_t* data,int length);
};

#endif //DEMOFFMPEG_MEDIACODECAUDIO_H

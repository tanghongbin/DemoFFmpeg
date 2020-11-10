//
// Created by Admin on 2020/11/9.
//

#ifndef DEMOC_TIMEASYNCHELPER_H
#define DEMOC_TIMEASYNCHELPER_H


#define DELAY_MAX_TIME 100

#include <cstdint>


extern "C"{
#include <libavutil/time.h>
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
};



class TimeAsyncHelper {

private:
    //开始播放的时间点
    long long startTime = -1L;
    // 当前播放的时间戳
    long curStamp;
public:


    /**
     * 判断解码时间戳是否有效
     * @return
     */
    bool assertDtsIsValid();

    /**
     * 向系统时间同步，快了就休息会，最多100ms，延迟如果超过100ms，丢弃
     * @return
     */
    long async();

    /**
     * 更新当前时间戳，dts或者pts
     * @param isPkt
     */
    void
    updateTimeStamp(bool isPkt, AVPacket *pkt, AVFrame *frame, AVFormatContext *pContext, int i);

};


#endif //DEMOC_TIMEASYNCHELPER_H

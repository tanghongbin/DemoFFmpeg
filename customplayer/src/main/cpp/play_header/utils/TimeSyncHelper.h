//
// Created by Admin on 2021/5/28.
//

#ifndef DEMOFFMPEG_TIMESYNCHELPER_H
#define DEMOFFMPEG_TIMESYNCHELPER_H

extern "C" {
#include <libavformat/avformat.h>
#include <libavutil/time.h>
};

class TimeSyncHelper{

private:
    int64_t startSysTime;
    int64_t pastDuration;
public:

    void resetTime();

    bool syncTime(bool isPkt,AVPacket* packet,AVFrame* frame,AVFormatContext* formatContext,int streamIndex);

    TimeSyncHelper();

};

#endif //DEMOFFMPEG_TIMESYNCHELPER_H

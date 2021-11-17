//
// Created by Admin on 2021/5/28.
//

#ifndef DEMOFFMPEG_TIMESYNCHELPER_H
#define DEMOFFMPEG_TIMESYNCHELPER_H

extern "C" {
#include <libavformat/avformat.h>
#include <libavutil/time.h>
};

typedef struct _timesyncbean{
    int64_t currentAudioPts;
    int type; // 1- 系统时间戳，2-视频 -> 音频，视频对照音频同步
} TimeSyncBean;

class TimeSyncHelper{

private:
    int64_t startSysTime;
    int64_t pastDuration;
public:

    void resetTime();

    bool syncTime(bool isPkt,AVPacket* packet,AVFrame* frame,AVFormatContext* formatContext,int streamIndex,TimeSyncBean* syncBean);

    TimeSyncHelper();

    bool syncBySysTime(bool isPkt, const AVPacket *packet, const AVFrame *frame,
                       const AVFormatContext *formatCtx, int streamIndex);

    bool syncByAudio(bool isPkt, const AVPacket *packet, const AVFrame *frame,
                     const AVFormatContext *formatCtx, int streamIndex,
                     const TimeSyncBean *syncBean) const;
};

#endif //DEMOFFMPEG_TIMESYNCHELPER_H

//
// Created by Admin on 2021/5/28.
//

#ifndef DEMOFFMPEG_TIMESYNCHELPER_H
#define DEMOFFMPEG_TIMESYNCHELPER_H

#include <media/NdkMediaCodec.h>

extern "C" {
#include <libavformat/avformat.h>
#include <libavutil/time.h>
};

typedef struct _timesyncbean{
    /***
     * 这个音频是当作比较标准的,都是毫秒
     */
    int64_t currentAudioPtsMs;
    /**
     * 播放中的视频pts值，仅当硬解码有效,毫秒为单位
     */
    int64_t currentVideoPtsMs;
    int syncType; // 1- 系统时间戳，2-视频 -> 音频，视频对照音频同步
} TimeSyncBean;

class TimeSyncHelper{

private:
    int64_t startSysTime;
    int64_t pastDuration;

    bool syncBySysTime(bool isPkt, const AVPacket *packet, const AVFrame *frame,
                       const AVFormatContext *formatCtx, int streamIndex);

    bool syncByAudio(bool isPkt, const AVPacket *packet, const AVFrame *frame,
                     const AVFormatContext *formatCtx, int streamIndex,
                     const TimeSyncBean *syncBean) const;

    bool hardwareSyncByAudio(bool isAudio,TimeSyncBean *syncBean);

    bool hardwareSyncBySysTime(bool isAudio,TimeSyncBean *syncBean);

public:

    void resetTime();

    /***
     * 软编码同步时间
     * @param isPkt
     * @param packet
     * @param frame
     * @param formatContext
     * @param streamIndex
     * @param syncBean
     * @return
     */
    bool syncTime(bool isPkt,AVPacket* packet,AVFrame* frame,AVFormatContext* formatContext,int streamIndex,TimeSyncBean* syncBean);
    /***
     * 硬解码同步时间
     * @return
     */
    bool hardwareSyncTime(bool isAudio,TimeSyncBean *syncBean);

    TimeSyncHelper();
};

#endif //DEMOFFMPEG_TIMESYNCHELPER_H

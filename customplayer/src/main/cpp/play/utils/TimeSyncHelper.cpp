//
// Created by Admin on 2021/5/28.
//

#include <utils/TimeSyncHelper.h>
#include <utils/utils.h>
#include <utils/CustomGLUtils.h>
#include <cmath>
#include <media/NdkMediaCodec.h>


#define MAX_WAIT_TIME 100

void TimeSyncHelper::resetTime(){
    pastDuration = 0L;
//    LOGCATE("log resetTime:%lld",pastDuration);
}

TimeSyncHelper::TimeSyncHelper(){
    startSysTime = pastDuration = 0L;
}

bool TimeSyncHelper::syncTime(bool isPkt,AVPacket* packet,AVFrame* frame,AVFormatContext* formatCtx,int streamIndex,
TimeSyncBean* syncBean){
    if (syncBean->syncType == 1) {
        return syncBySysTime(isPkt, packet, frame, formatCtx, streamIndex);
    } else {
        // 视频向音频同步
        return syncByAudio(isPkt, packet, frame, formatCtx, streamIndex, syncBean);
    }
}

bool TimeSyncHelper::hardwareSyncTime(bool isAudio,
                              TimeSyncBean *syncBean){
    if (syncBean->syncType == 1) {
        return hardwareSyncBySysTime(isAudio, syncBean);
    } else {
        // 视频向音频同步
        return hardwareSyncByAudio(isAudio, syncBean);
    }
}

bool TimeSyncHelper::syncByAudio(bool isPkt, const AVPacket *packet, const AVFrame *frame,
                                 const AVFormatContext *formatCtx, int streamIndex,
                                 const TimeSyncBean *syncBean) const {
    if (formatCtx->streams[streamIndex]->codecpar->codec_type != AVMEDIA_TYPE_VIDEO) return true;

    int64_t avDuration = 0L;
    bool frameIsValid = true;
    if (isPkt) {
        avDuration = packet->dts * av_q2d(formatCtx -> streams[streamIndex]->time_base) * 1000;
    } else {
        if (frame->pkt_dts != AV_NOPTS_VALUE){
            avDuration = frame->pkt_dts * av_q2d(formatCtx->streams[streamIndex]->time_base) * 1000;
        } else if (frame->pts != AV_NOPTS_VALUE) {
            avDuration = frame->pts * av_q2d(formatCtx->streams[streamIndex]->time_base) * 1000;
        } else {
            avDuration = 0L;
        }
    }
    int64_t sysDuration = syncBean->currentAudioPts;
    // 统一以毫秒作计算单位
    if (avDuration != 0L) {
        int64_t waitSysTime = avDuration - sysDuration;
        if (waitSysTime > 0) {
            waitSysTime = waitSysTime > MAX_WAIT_TIME ? MAX_WAIT_TIME : waitSysTime;
            av_usleep(waitSysTime * 1000);
        } else if (abs(waitSysTime) > MAX_WAIT_TIME) {
            frameIsValid = false;
        }
//        LOGCATE("log sysDuration:%lld avduration:%lld waitSys:%lld",sysDuration,avDuration,waitSysTime);
    }
    return frameIsValid;
}


bool TimeSyncHelper::hardwareSyncByAudio(bool isAudio,TimeSyncBean *syncBean) {
    if (isAudio) return true;
    int64_t avDuration = syncBean->currentVideoPts;
    bool frameIsValid = true;
    int64_t sysDuration = syncBean->currentAudioPts;
    // 统一以毫秒作计算单位
    if (avDuration != 0L) {
        int64_t waitSysTime = avDuration - sysDuration;
        if (waitSysTime > 0) {
            waitSysTime = waitSysTime > MAX_WAIT_TIME ? MAX_WAIT_TIME : waitSysTime;
            av_usleep(waitSysTime * 1000);
        } else if (abs(waitSysTime) > MAX_WAIT_TIME) {
            frameIsValid = false;
        }
//        LOGCATE("当前同步结果:%d   音频:%lld   视频:%lld",frameIsValid,sysDuration,avDuration);
//        LOGCATE("log sysDuration:%lld avduration:%lld waitSys:%lld",sysDuration,avDuration,waitSysTime);
    }
    return frameIsValid;
}

bool TimeSyncHelper::syncBySysTime(bool isPkt, const AVPacket *packet, const AVFrame *frame,
                                   const AVFormatContext *formatCtx, int streamIndex) {
    if (startSysTime == 0L) startSysTime = GetSysCurrentTime();
    if (formatCtx->streams[streamIndex]->codecpar->codec_type != AVMEDIA_TYPE_VIDEO) return true;

    int64_t avDuration = 0L;
    bool frameIsValid = true;
    if (isPkt) {
        avDuration = packet->dts * av_q2d(formatCtx -> streams[streamIndex]->time_base) * 1000;
    } else {
        if (frame->pkt_dts != AV_NOPTS_VALUE){
            avDuration = frame->pkt_dts * av_q2d(formatCtx->streams[streamIndex]->time_base) * 1000;
        } else if (frame->pts != AV_NOPTS_VALUE) {
            avDuration = frame->pts * av_q2d(formatCtx->streams[streamIndex]->time_base) * 1000;
        } else {
            avDuration = 0L;
        }
    }
    if (pastDuration == 0L){
        pastDuration = avDuration;
        startSysTime = GetSysCurrentTime() - pastDuration;
    } else {
        pastDuration = avDuration;
    }
    int64_t sysDuration = (GetSysCurrentTime() - startSysTime);
    // 统一以毫秒作计算单位
    if (avDuration != 0L) {
        int64_t waitSysTime = avDuration - sysDuration;
        if (waitSysTime > 0) {
            waitSysTime = waitSysTime > MAX_WAIT_TIME ? MAX_WAIT_TIME : waitSysTime;
            av_usleep(waitSysTime * 1000);
        } else if (abs(waitSysTime) > MAX_WAIT_TIME) {
            frameIsValid = false;
        }
        pastDuration = avDuration;
//        LOGCATE("log sysDuration:%lld avduration:%lld waitSys:%lld",sysDuration,avDuration,waitSysTime);
    }
    return frameIsValid;
}


bool TimeSyncHelper::hardwareSyncBySysTime(bool isAudio,TimeSyncBean *syncBean) {
    if (isAudio) return true;
    if (startSysTime == 0L) startSysTime = GetSysCurrentTime();

    int64_t avDuration = syncBean->currentVideoPts;
    bool frameIsValid = true;
    if (pastDuration == 0L){
        pastDuration = avDuration;
        startSysTime = GetSysCurrentTime() - pastDuration;
    } else {
        pastDuration = avDuration;
    }
    int64_t sysDuration = (GetSysCurrentTime() - startSysTime);
    // 统一以毫秒作计算单位
    if (avDuration != 0L) {
        int64_t waitSysTime = avDuration - sysDuration;
        if (waitSysTime > 0) {
            waitSysTime = waitSysTime > MAX_WAIT_TIME ? MAX_WAIT_TIME : waitSysTime;
            av_usleep(waitSysTime * 1000);
        } else if (abs(waitSysTime) > MAX_WAIT_TIME) {
            frameIsValid = false;
        }
        pastDuration = avDuration;
//        LOGCATE("当前同步结果:%d   系统时间:%lld   视频:%lld  差值:%lld",frameIsValid,sysDuration,avDuration,waitSysTime);
//        LOGCATE("log sysDuration:%lld avduration:%lld waitSys:%lld",sysDuration,avDuration,waitSysTime);
    }
    return frameIsValid;
}
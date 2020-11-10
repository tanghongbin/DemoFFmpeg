//
// Created by Admin on 2020/11/9.
//


#include "TimeAsyncHelper.h"
#include "CustomGLUtils.h"




bool TimeAsyncHelper::assertDtsIsValid(){
    return async() <= DELAY_MAX_TIME;
}

long TimeAsyncHelper::async(){
    // 系统过去的时间
    long long curTime = GetSysCurrentTime();
    long long eapseTime = curTime - startTime;
    long delay = 0;

//    auto differss = static_cast<unsigned int>(curStamp - eapseTime);
//    if (differss >= 100){
//        LOGCATE("sysCurTime:%lld startTime:%lld pastSysDuration:%lld \n"
//                "videoCurTime:%ld syncDiffer:%d",curTime,startTime,eapseTime,curStamp,differss);
//    }

//    LOGCATE("print timebase:%",av_q2d(*timeBase));
    if (curStamp > eapseTime){
        auto differ = static_cast<unsigned int>(curStamp - eapseTime);
        differ = differ > DELAY_MAX_TIME ? DELAY_MAX_TIME : differ;
        LOGCATE("videoCurTime:%ld pastSysDuration:%lld syncDiffer:%d",curStamp,eapseTime,differ);
        av_usleep(differ * 1000);
    }
    delay = static_cast<long>(eapseTime - curStamp);
    return delay;
}

void TimeAsyncHelper::updateTimeStamp(bool isPkt, AVPacket *pkt, AVFrame *frame,
                                      AVFormatContext *pContext, int stream_index) {
    if (startTime == -1L) {
        startTime = GetSysCurrentTime();
    }

    if (isPkt){
        if (pkt->dts > 0 && pkt->dts != AV_NOPTS_VALUE){
            curStamp = static_cast<long int>(pkt->dts * av_q2d(pContext->streams[stream_index]->time_base) * 1000);
            LOGCATE("pkt's dts:%lld dts_value:%ld",pkt->dts,curStamp);
        }
//        LOGCATE("print pkt-dts:%lld",pkt->dts);
    } else {
        if (frame->pkt_dts != AV_NOPTS_VALUE){
            curStamp = static_cast<long int>(frame->pkt_dts * av_q2d(pContext->streams[stream_index]->time_base) * 1000);
//            LOGCATE("frame's pkt_dts:%lld pkt_dts_value:%ld",frame->pkt_dts,curStamp);
        } else if (frame->pts != AV_NOPTS_VALUE){
            curStamp = static_cast<long int>(frame->pts * av_q2d(pContext->streams[stream_index]->time_base) * 1000);
//            LOGCATE("frame's pts:%lld pts_value:%ld",frame->pts,curStamp);
        } else {
            curStamp = 0;
            LOGCATE("frame's pkt_dts and pts is invalid");
        }
//        LOGCATE("print frame-pkt_dts:%lld frame-pts",frame->pkt_dts,frame->pts);
    }
}
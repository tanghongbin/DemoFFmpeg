//
// Created by Admin on 2021/8/24.
//

#ifndef DEMOFFMPEG_RTMPPUSHHELPER_H
#define DEMOFFMPEG_RTMPPUSHHELPER_H

#include <cstdlib>
#include <thread>
#include <utils/CustomSafeBlockQueue.h>
#include <rtmp/rtmp.h>
#include <utils/Constants.h>

using std::thread;

/***
 * rtmp 推流器 发送音频和视频数据, 顺带组装数据
 */
class RtmpPushHelper {


private:
    bool isPushing;
    thread* rtmpThread;
    static void loopRtmpPush(RtmpPushHelper* pushHelper);
    CustomSafeBlockQueue<RTMPPacket*> packetQueue;
    int64_t startTime;
    RTMP * mRtmp;
    char rtmpUrl[128];

public:

    RtmpPushHelper(){
        startTime = 0L;
        rtmpThread = 0;
        isPushing = true;
        mRtmp = 0;
    }

    ~RtmpPushHelper(){
    }
    void initPush();
    void putVideoSpsPps(uint8_t* sps,uint8_t* pps,int spslen,int ppslen);
    void putVideoBody(int type,uint8_t* payload,int i_payload);
    void destroy();
    void putAacPacket(const uint8_t *outputAudioBuffer, int byteLen);
    void putAudioTagFirst(void *audioEncodec);

    void initTime();

    static void log_rtmp_infos(int level, const char *fmt, va_list args);
};


#endif //DEMOFFMPEG_RTMPPUSHHELPER_H

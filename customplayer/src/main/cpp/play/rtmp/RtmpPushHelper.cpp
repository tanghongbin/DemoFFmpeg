//
// Created by Admin on 2021/8/24.
//

#include "../../play_header/rtmp/RtmpPushHelper.h"
#include <rtmp/rtmp.h>
#include <rtmp/http.h>
#include <rtmp/amf.h>
#include <rtmp/log.h>
#include <utils/CustomGLUtils.h>
#include <x264/x264.h>

#define RTMPURL "rtmp://1.14.99.52:1935/live/androidPush"


void RtmpPushHelper::initPush(){
    rtmpThread = new thread(RtmpPushHelper::loopRtmpPush, this);
}

#pragma clang diagnostic push
#pragma ide diagnostic ignored "hicpp-signed-bitwise"
void RtmpPushHelper::putVideoSpsPps(uint8_t* sps,uint8_t* pps,int spslen,int ppslen) {
    int bodySize = 13 + spslen + 3 + ppslen;
    auto * packet = new RTMPPacket ;
    RTMPPacket_Alloc(packet,bodySize);
    int i = 0;
    // start code
    packet->m_body[i++] = 0x17;
    // type
    packet->m_body[i++] = 0x00;
    packet->m_body[i++] = 0x00;
    packet->m_body[i++] = 0x00;
    packet->m_body[i++] = 0x00;

    //version
    packet->m_body[i++] = 0x01;
    packet->m_body[i++] = sps[1];
    packet->m_body[i++] = sps[2];
    packet->m_body[i++] = sps[3];
    packet->m_body[i++] = 0xFF;

    //sps
    packet->m_body[i++] = 0xE1;
    packet->m_body[i++] = (spslen >> 8) & 0xff;
    packet->m_body[i++] = spslen & 0xff;
    memcpy(&packet->m_body[i],sps,spslen);
    i += spslen;


    // pps
    packet->m_body[i++] = 0x01;
    packet->m_body[i++] = (ppslen >> 8) & 0xff;
    packet->m_body[i++] = ppslen & 0xff;
    memcpy(&packet->m_body[i],pps,ppslen);
    LOGCATE("打印spsLen:%d  ppsLen:%d",spslen,ppslen);

    // video
    packet->m_packetType = RTMP_PACKET_TYPE_VIDEO;
    packet->m_nBodySize = bodySize;
    packet->m_nChannel = 0x10;
    // sps and pps no timestamp
    packet->m_nTimeStamp = 0;
    packet->m_hasAbsTimestamp = 0;
    packet->m_nInfoField2 = mRtmp->m_stream_id;
    packet->m_headerType = RTMP_PACKET_SIZE_MEDIUM;
    initTime();
    packet->m_nTimeStamp = RTMP_GetTime() - startTime;
    LOGCATE("log putVideoSpsPps pkt is ready:%d",RTMPPacket_IsReady(packet));
    packetQueue.pushLast(packet);
    RTMPPacket_Dump(packet);
    LOGCATE("has put sps pps info");
}
#pragma clang diagnostic pop
#pragma clang diagnostic push
#pragma ide diagnostic ignored "hicpp-signed-bitwise"
void RtmpPushHelper::putVideoBody(int type,uint8_t* payload,int i_payload) {
    if (payload[2] == 0x00){
        i_payload -= 4;
        payload += 4;
    } else {
        i_payload -= 3;
        payload += 3;
    }
    int bodySize = i_payload + 9;
    auto * packet = new RTMPPacket ;
    RTMPPacket_Alloc(packet,bodySize);
    packet->m_body[0] = 0x27;
    if (type == NAL_SLICE_IDR){
        packet->m_body[0] = 0x17;
    }
    packet->m_body[1] = 0x01;
    // timestamp
    packet->m_body[2] = 0x00;
    packet->m_body[3] = 0x00;
    packet->m_body[4] = 0x00;
    // packet len
    packet->m_body[5] = (i_payload >> 24) & 0xff;
    packet->m_body[6] = (i_payload >> 16) & 0xff;
    packet->m_body[7] = (i_payload >> 8) & 0xff;
    packet->m_body[8] = (i_payload) & 0xff;

    memcpy(&packet->m_body[9],payload,(size_t)i_payload);
    LOGCATE("打印 body_size:%d",i_payload);

    packet->m_nInfoField2 = mRtmp->m_stream_id;
    packet->m_hasAbsTimestamp = 0;
    packet->m_nBodySize = bodySize;
    packet->m_packetType = RTMP_PACKET_TYPE_VIDEO;
    packet->m_nChannel = 0x10;
    packet->m_headerType = RTMP_PACKET_SIZE_LARGE;
    initTime();
    packet->m_nTimeStamp = RTMP_GetTime() - startTime;
    LOGCATE("log putVideoBody pkt is ready:%d",RTMPPacket_IsReady(packet));
    packetQueue.pushLast(packet);
    RTMPPacket_Dump(packet);
//    LOGCATE("has put video body info");
}
#pragma clang diagnostic pop
void RtmpPushHelper::loopRtmpPush(RtmpPushHelper* instance){
    instance->mRtmp = RTMP_Alloc();
    RTMP * rtmp = instance->mRtmp;
    const char * errorMsg = nullptr;
    uint32_t startTime;
    int ret;
    if (!rtmp) {
        errorMsg = "RTMP_Alloc failed";
        goto RtmpPushEnd;
    }
    RTMP_Init(rtmp);
    RTMP_LogSetLevel(RTMP_LOGERROR);
    RTMP_LogSetCallback(log_rtmp_infos);
    ret = RTMP_SetupURL(rtmp,RTMPURL);
    rtmp->Link.timeout = 5;// 秒为单位
    RTMP_EnableWrite(rtmp);
    ret = RTMP_Connect(rtmp,0);
    if (!ret){
        errorMsg = "RTMP_Connect failed";
        goto RtmpPushEnd;
    }
    ret = RTMP_ConnectStream(rtmp,0);
    if (!ret){
        errorMsg = "RTMP_ConnectStream failed";
        goto RtmpPushEnd;
    }
    startTime = RTMP_GetTime();
    while (instance -> isPushing){
        RTMPPacket *packet = instance->packetQueue.popFirst();
        if (packet == nullptr) {
            usleep(1000 * 5);
            continue;
        }
        packet->m_nInfoField2 = rtmp->m_stream_id;
        if (!RTMP_IsConnected(rtmp)){
            errorMsg = "RTMP_SendPacket failed, has disconnected";
            break;
        }
        ret = RTMP_SendPacket(rtmp,packet,1);
        RTMPPacket_Reset(packet);
        RTMPPacket_Free(packet);
        delete packet;
        // 先不判断，可能有延迟，推送不成功什么的
        if (!ret){
            errorMsg = "RTMP_SendPacket is failed";
            break;
        }
    }

    // 结束
    RtmpPushEnd:
    if (errorMsg) LOGCATE("打印rtmppush 的错误:%s",errorMsg);
    if (rtmp) {
        RTMP_Close(rtmp);
        RTMP_Free(rtmp);
    }
    LOGCATE(" push rtmp is over");
}
void RtmpPushHelper::destroy(){
    isPushing = false;
    packetQueue.pushLast(nullptr);
    if (rtmpThread) {
        rtmpThread->join();
        delete rtmpThread; 
    }
    packetQueue.pushLast(nullptr);
    do {
        RTMPPacket *item = packetQueue.popFirst();
        if (item){
            RTMPPacket_Free(item);
            delete item;
        }
    } while (packetQueue.size() > 0);
    LOGCATE("rtmpPushHelper all destroy");
}

void RtmpPushHelper::initTime() {
    if (startTime == 0L) {
        startTime = RTMP_GetTime();
    }
}

void RtmpPushHelper::log_rtmp_infos(int level, const char *msg, va_list args) {
    char log[2048];
    vsprintf(log, msg, args);
    LOGCATE("log rtmp info ------ %d %s",level,log);
}

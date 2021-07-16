//
// Created by Admin on 2021/1/7.
//


#include <utils.h>
#include <CustomSafeQueue.h>
#include "RtmpClient.h"

extern "C" {
#include <rtmp.h>
#include <libavutil/time.h>
}

RtmpClient *RtmpClient::instance = 0;

RtmpClient *RtmpClient::getInstance() {
    if (instance == nullptr) {
        instance = new RtmpClient;
    }
    return instance;
}


void RtmpClient::destroyInstance() {
    if (instance != nullptr) {
        delete instance;
        instance = nullptr;
    }
}

void RtmpClient::loop(RtmpClient *client) {
    LOGCATE("start rtmp loop");
    while (client->isStart) {
        RTMPPacket *packet = client->mDataQueue.popFirst();
        if (packet == 0) {
            LOGCATE("pkt is null,this is impossible");
            av_usleep(10 * 1000);
            continue;
        }
        packet->m_nInfoField2 = client->rtmp->m_stream_id;
//        LOGCATE("send pkt info body size:%d  type:%d",packet->m_nBodySize,packet->m_packetType);
        int ret = RTMP_SendPacket(client->rtmp, packet, 1);
//        LOGCATE("发送结果：%d",ret);
        if (!ret) {
            LOGCATE("发送失败:%d", ret);
            break;
        }
    }
    LOGCATE("loop end");
    if (client->mDataQueue.size() != 0){
        RTMPPacket *deletePkt = client->mDataQueue.removeFirst();
        while (deletePkt != 0) {
            RTMPPacket_Free(deletePkt);
            delete deletePkt;
            deletePkt = client->mDataQueue.removeFirst();
        }
    }
    LOGCATE("rtmp loop end :%d", (nullptr == 0));
}

void RtmpClient::initRtmp(char *url) {
//初始化
    rtmp = RTMP_Alloc();
    if (!rtmp) {
        LOGCATE("rtmp init failed");
        return;
    }
    RTMP_Init(rtmp);
    //设置地址
    int ret = RTMP_SetupURL(rtmp, url);
    if (!ret) {
        LOGCATE("rtmp setup url failed");
        return;
    }

    //设置超时时间 单位 5
    rtmp->Link.timeout = 5;
    RTMP_EnableWrite(rtmp);

    ret = RTMP_Connect(rtmp, 0);
    if (!ret) {
        LOGCATE("connect rtmp failed:%d", ret);
        return;
    }

    ret = RTMP_ConnectStream(rtmp, 0);
    if (!ret) {
        LOGCATE("RTMP_ConnectStream failed:%d", ret);
        return;
    }
    //记录一个开始时间
    startTime = RTMP_GetTime();
    //表示可以开始推流了
    isStart = true;

    LOGCATE("rtmp connected successed address:%s", url);
    //死循环阻塞获取推流数据
    thread = new std::thread(loop, this);
}

void RtmpClient::destroyRtmp() {
    isStart = false;
    if (rtmp) {
        LOGCATE("begin to destroy");
        mDataQueue.pushLast(0);
        thread->join();
        thread = 0;
        RTMP_DeleteStream(rtmp);
        RTMP_Close(rtmp);
        RTMP_Free(rtmp);
        rtmp = 0;
        LOGCATE("释放 native 资源");
    }
}

void RtmpClient::sendData(uint8_t *data, int type, int dataSize) {
    if (type == FIRST_AUDIO || type == AUDIO) {//音频数据
        pushAACData(data, dataSize);
    } else if (type == FIRST_VIDEO || type == INTER_FRAME || type == KEY_FRAME) {
        //视频数据
        pushH264(data, dataSize);
    } else if (type == PCM) {
//        mPusherManager.pushPCM(data);
    } else if (type == YUV) {
//        mPusherManager.pushYUV(data);
    }
}

void RtmpClient::pushAACData(uint8_t *data, int dataLen) {
    RTMPPacket *packet = (RTMPPacket *) malloc(sizeof(RTMPPacket));
    RTMPPacket_Alloc(packet, dataLen);
    RTMPPacket_Reset(packet);
    packet->m_nChannel = 0x05; //音频
    memcpy(packet->m_body, data, dataLen);
    packet->m_headerType = RTMP_PACKET_SIZE_LARGE;
    packet->m_hasAbsTimestamp = FALSE;
    packet->m_packetType = RTMP_PACKET_TYPE_AUDIO;
    packet->m_nBodySize = dataLen;
    pushData(packet);
}

void RtmpClient::pushData(RTMPPacket *packet) {
    packet->m_nTimeStamp = RTMP_GetTime() - startTime;
    mDataQueue.pushLast(packet);
}

void RtmpClient::pushH264(uint8_t *data, int dataLen) {
    RTMPPacket *packet = (RTMPPacket *) malloc(sizeof(RTMPPacket));
    RTMPPacket_Alloc(packet, dataLen);
    RTMPPacket_Reset(packet);

    packet->m_nChannel = 0x04; //视频

//    if (type == KEY_FRAME) {
//        LOGCATE("视频关键帧");
//    }

    memcpy(packet->m_body, data, dataLen);
    packet->m_headerType = RTMP_PACKET_SIZE_LARGE;
    packet->m_hasAbsTimestamp = FALSE;
    packet->m_packetType = RTMP_PACKET_TYPE_VIDEO;
    packet->m_nBodySize = dataLen;
    pushData(packet);
//    mVideoCallback(packet);
}


//
// Created by Admin on 2021/1/7.
//

#include <rtmp.h>
#include <utils.h>
#include "RtmpClient.h"

RtmpClient* RtmpClient::getInstance() {
    if (instance == nullptr){
        instance = new RtmpClient;
    }
    return instance;
}


void RtmpClient::destroyInstance() {
    if (instance != nullptr){
        delete instance;
        instance = nullptr;
    }
}

void RtmpClient::loop(RtmpClient* client) {
    while (client -> isStart){

    }
    LOGCATE("rtmp loop end");
}

void RtmpClient::initRtmp(char* url) {
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
        LOGCATE("connect rtmp failed:%d",ret);
        return;
    }

    ret = RTMP_ConnectStream(rtmp, 0);
    if (!ret) {
        LOGCATE("RTMP_ConnectStream failed:%d",ret);
        return;
    }
    //记录一个开始时间
    startTime = RTMP_GetTime();
    //表示可以开始推流了
    isStart = true;

    //死循环阻塞获取推流数据
    thread = new std::thread(loop);
}

void RtmpClient::destroyRtmp() {
    isStart = false;
    if (rtmp) {
        RTMP_DeleteStream(rtmp);
        RTMP_Close(rtmp);
        RTMP_Free(rtmp);
        rtmp = 0;
        LOGCATE("释放 native 资源");
    }
}

void RtmpClient::sendData(uint8_t *data, int type)  {

}


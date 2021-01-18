//
// Created by Admin on 2021/1/7.
//

#ifndef DEMOC_RTMPCLIENT_H
#define DEMOC_RTMPCLIENT_H


extern "C"{
#include <rtmp.h>
};

#include <thread>
#include <CustomSafeQueue.h>



#define FIRST_VIDEO  1
#define  FIRST_AUDIO  2
#define  AUDIO  3
#define  KEY_FRAME  4
#define  INTER_FRAME  5
#define  CONFIGRATION  6
#define  PCM  7
#define  YUV  8

class RtmpClient {

private:
    static RtmpClient* instance;
    uint32_t startTime;
    std::thread *thread;
    RTMP *rtmp;
    static void loop(RtmpClient* client);
    bool isStart = true;
    CustomSafeQueue<RTMPPacket* > mDataQueue;


public:



    static RtmpClient* getInstance();

    static void destroyInstance();

    void initRtmp(char* url);

    void destroyRtmp();

    void sendData(uint8_t* data,int type,int size);

    void pushAACData(uint8_t *data, int dataLen);

    void pushH264(uint8_t *data, int dataLen);

    void pushData(RTMPPacket *packet);
};


#endif //DEMOC_RTMPCLIENT_H

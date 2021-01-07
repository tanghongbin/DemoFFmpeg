//
// Created by Admin on 2021/1/7.
//

#ifndef DEMOC_RTMPCLIENT_H
#define DEMOC_RTMPCLIENT_H


#include <thread>
#include "../../../../../../../../android_sdk/android_sdk/sdk/ndk-bundle/toolchains/llvm/prebuilt/windows-x86_64/sysroot/usr/include/c++/v1/cstdint"

class RtmpClient {

private:
    static RtmpClient* instance;
    uint32_t startTime;
    std::thread *thread;
    RTMP *rtmp;
    static void loop(RtmpClient* client);


public:
    bool isStart = true;

    static RtmpClient* getInstance();

    static void destroyInstance();

    void initRtmp(char* url);

    void destroyRtmp();

    void sendData(uint8_t* data,int type);
};


#endif //DEMOC_RTMPCLIENT_H

//
// Created by Admin on 2021/5/13.
//

#ifndef DEMOFFMPEG_ABSCUSTOMMEDIAPLAYER_H
#define DEMOFFMPEG_ABSCUSTOMMEDIAPLAYER_H

#include <cstdint>
#include <utils/CustomSafeBlockQueue.h>
#include <utils/Message.h>
#include <utils/JavaVmManager.h>
#include <thread>
#include <mutex>
#include <utils/MsgLoopHelper.h>

class AbsCustomMediaPlayer{

private:
    std::mutex absMutex;
    int prepareCount;

protected:
    char mUrl[1024];
    static void prepareReady(long mediaPlayerLong){
        AbsCustomMediaPlayer* mediaPlayer = reinterpret_cast<AbsCustomMediaPlayer *>(mediaPlayerLong);
        std::lock_guard<std::mutex> uniqueLock(mediaPlayer->absMutex);
        mediaPlayer->prepareCount++;
        LOGCATE("onPrepared send msg success %d",mediaPlayer->prepareCount);
        if (mediaPlayer->prepareCount == 1){
            MsgLoopHelper::sendMsg(Message::obtain(JNI_COMMUNICATE_TYPE_PREPARED,0,0));
        }
    }
public:
    virtual void Init() = 0;
    virtual void OnSurfaceCreated() {  };
    virtual void OnSurfaceChanged(int oreration,int width,int height) {  };
    virtual void OnDrawFrame() {  };
    virtual void DecodeFrame(uint8_t* data) {};
    virtual void Destroy() = 0;
    virtual void SetDataUrl(const char * url) {
        strcpy(mUrl,url);
        LOGCATE("打印地址:%s",mUrl);
    };
    virtual void Prepare() = 0;
    virtual void Start() = 0;
    virtual void Stop() = 0;
    virtual void SeekTo(int second) = 0;
    AbsCustomMediaPlayer(){
        prepareCount = 0;
    }
    virtual void Replay(){};
};

#endif //DEMOFFMPEG_ABSCUSTOMMEDIAPLAYER_H

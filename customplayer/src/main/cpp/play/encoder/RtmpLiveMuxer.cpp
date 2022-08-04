//
// Created by Admin on 2021/5/31.
//



#include <utils/utils.h>
#include <utils/CustomGLUtils.h>
#include <render/VideoRender.h>
#include <libyuv.h>
#include <libyuv/scale.h>
#include <libyuv/convert.h>
#include <libyuv/rotate.h>
#include <render/VideoFboRender.h>
#include "../../play_header/encoder/RtmpLiveMuxer.h"


RtmpLiveMuxer* RtmpLiveMuxer::instance = nullptr;

/**************************************************************/

/* media file output */

RtmpLiveMuxer::RtmpLiveMuxer(){
    audioTrackIndex = videoTrackIndex = 0;
    isDestroyed = false;
    mAvEncoder = 0;
    isStartEncode = false;
    isInit = false;
}

int RtmpLiveMuxer::init(const char * fileName){
    if (isInit) return 0;
    Constants::rtmpPushUrl = fileName;
    mAvEncoder = new EncoderAACAndx264;
    mAvEncoder->init();
    LOGCATE("打印复制名字:%s      resultName:%s",fileName,Constants::rtmpPushUrl);
    isInit = true;
    return 0;
}


void RtmpLiveMuxer::Start() {
    isStartEncode = true;
}

void RtmpLiveMuxer::Stop() {
    isStartEncode = false;
}


void RtmpLiveMuxer::receivePixelData(int type,NativeOpenGLImage *src){
    if (!getInstance()->isStartEncode) {
        NativeOpenGLImageUtil::FreeNativeImage(src);
        delete src;
        return;
    }
    getInstance() -> mAvEncoder->putVideoData(src);
}

void RtmpLiveMuxer::receiveAudioData(short *audioData, int length){
    if (!getInstance() ->isStartEncode) return;
    std::unique_lock<std::mutex> uniqueLock(RtmpLiveMuxer::getInstance() -> runningMutex,std::defer_lock);
    uniqueLock.lock();
    if (getInstance() ->isDestroyed){
        uniqueLock.unlock();
        return;
    }
    uniqueLock.unlock();

    auto * data = new uint8_t [length];
    memcpy(data,audioData,length);
    getInstance() ->mAvEncoder->putAudioData(data, length);
    // 处理数据
}

void RtmpLiveMuxer::configAudioPrams(int samHz,int chnns){

}

void RtmpLiveMuxer::getInAvDataFunc(ReceiveAvOriginalData* data) {
    data->audioCall = reinterpret_cast<ReceiveAudioData>(RtmpLiveMuxer::receiveAudioData);
    data->videoCall = RtmpLiveMuxer::receivePixelData;
}


void RtmpLiveMuxer::Destroy(){
    // 清空所有缓存音频
    std::unique_lock<std::mutex> uniqueLock(runningMutex,std::defer_lock);
    uniqueLock.lock();
    isDestroyed = true;
    uniqueLock.unlock();
    if (mAvEncoder){
        mAvEncoder->destroy();
        delete mAvEncoder;
    }
    delete instance;
    instance = 0;
    LOGCATE("rtmplivemuxer destroy over");
}
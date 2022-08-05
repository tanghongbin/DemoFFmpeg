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
    LOGCATE("start rtmp push");
}

void RtmpLiveMuxer::Stop() {
    isStartEncode = false;
}


void RtmpLiveMuxer::receivePixelData(int type,NativeOpenGLImage *src){
    if (!getInstance()->isStartEncode || !getInstance()->mAvEncoder) {
        return;
    }
    getInstance() -> mAvEncoder->putVideoData(src);
}

void RtmpLiveMuxer::receiveAudioData(short *audioData, int length){
    if (!getInstance() ->isStartEncode || getInstance() ->isDestroyed || !getInstance()->mAvEncoder) {
        return;
    }
    getInstance() ->mAvEncoder->putAudioData(reinterpret_cast<uint8_t *>(audioData), length);
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
    isDestroyed = true;
    if (mAvEncoder){
        mAvEncoder->destroy();
        delete mAvEncoder;
    }
    instance = nullptr;
    LOGCATE("rtmplivemuxer destroy over");
}
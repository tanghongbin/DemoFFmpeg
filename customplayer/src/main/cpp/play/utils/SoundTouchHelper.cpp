//
// Created by Admin on 2021/10/15.
//

#include "../../play_header/utils/SoundTouchHelper.h"


#define DEFAULT_AUDIO_TRACK 0

SoundTouchHelper::SoundTouchHelper(){
    resultData = 0;
    speed = 1.0;
}

void SoundTouchHelper::init(){
    int channels = DEFAULT_AUDIO_CHANNEL_COUNT
    int sampleRate = DEFAULT_AUDIO_FREQUENCY;
    LOGCATE("初始化音频变频时的速度：%lf",speed);
    SoundTouchUtils::getInstance()->initSpeedController(DEFAULT_AUDIO_TRACK,channels,sampleRate,speed,1.0);
    resultData = new SAMPLETYPE [DEFAULT_AUDIO_FRAME_SIZE_SINGLE * 2];
}

void SoundTouchHelper::setSpeed(double speed){
    this->speed = speed;
}

void SoundTouchHelper::setAudioCallback(ReceiveAudioData audio){
    this->audioCallback = audio;
}

void SoundTouchHelper::adjustAudioData(uint8_t* audio,int length){
    // 这里假设进来的都不是1.0倍的
    SoundTouchUtils::getInstance()->putData(DEFAULT_AUDIO_TRACK,audio,length);
    int frameSize = DEFAULT_AUDIO_FRAME_SIZE_SINGLE * 2;
    memset(resultData,0x00,frameSize);
    while (true){
        int size = SoundTouchUtils::getInstance()->getData(DEFAULT_AUDIO_TRACK,&resultData,length);
        if(size <= 0) break;
        audioCallback(resultData,size);
    }
}

void SoundTouchHelper::destroy(){
    delete [] resultData;
    SoundTouchUtils::getInstance()->close(DEFAULT_AUDIO_TRACK);
}
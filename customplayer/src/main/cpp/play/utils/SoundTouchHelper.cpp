//
// Created by Admin on 2021/10/15.
//

#include "../../play_header/utils/SoundTouchHelper.h"


#define DEFAULT_AUDIO_TRACK 0

SoundTouchHelper::SoundTouchHelper(){
    resultData = 0;
    isInit = false;
}

void SoundTouchHelper::init(){
    resultData = new SAMPLETYPE [DEFAULT_AUDIO_FRAME_SIZE_SINGLE * 2];
}

void SoundTouchHelper::setSpeed(double speed){
    if (isInit){
        SoundTouchUtils::getInstance()->close(DEFAULT_AUDIO_TRACK);
    }
    SoundTouchUtils::getInstance()->initSpeedController(DEFAULT_AUDIO_TRACK,DEFAULT_AUDIO_CHANNEL_COUNT,
            DEFAULT_AUDIO_FREQUENCY,speed,1.0);
    isInit = true;
}

void SoundTouchHelper::setAudioCallback(ReceiveAudioData audio){
    this->audioCallback = audio;
}

void SoundTouchHelper::adjustAudioData(uint8_t* audio,int length){
    if (!isInit) return;
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
    if (isInit) SoundTouchUtils::getInstance()->close(DEFAULT_AUDIO_TRACK);
}
//
// Created by Admin on 2021/10/15.
//

#include "../../play_header/utils/SoundTouchHelper.h"


#define DEFAULT_AUDIO_TRACK 2
#define AUDIO_TRACK_TOTAL 5
#define AV_SPEED_VERY_SLOW 0.25
#define AV_SPEED_SLOW 0.5
#define AV_SPEED_NORMAL 1.0
#define AV_SPEED_FAST 2.0
#define AV_SPEED_VERY_FAST 3.0

SoundTouchHelper::SoundTouchHelper(){
    resultData = 0;
    currentSpeedIndex = 0;
    isInit = false;
}

void SoundTouchHelper::init(){
    resultData = new SAMPLETYPE [DEFAULT_AUDIO_FRAME_SIZE_SINGLE * 2];
    double speedArray[5] = {AV_SPEED_VERY_SLOW,AV_SPEED_SLOW,AV_SPEED_NORMAL,
                            AV_SPEED_FAST,AV_SPEED_VERY_FAST};
    for (int i = 0; i < AUDIO_TRACK_TOTAL; ++i) {
        SoundTouchUtils::getInstance()->initSpeedController(i,DEFAULT_AUDIO_CHANNEL_COUNT,
                                                            DEFAULT_AUDIO_FREQUENCY,speedArray[i],1.0);
        speedIndexMap.insert(std::map<double ,int>::value_type(speedArray[i],i));
    }
    isInit = true;
}

void SoundTouchHelper::setSpeed(double speed){
    if (!isInit) return;
    const map<double, int>::iterator &item = speedIndexMap.find(speed);
    currentSpeedIndex = item->second;
}

void SoundTouchHelper::setAudioCallback(ReceiveAudioData audio){
    this->audioCallback = audio;
}

void SoundTouchHelper::adjustAudioData(uint8_t* audio,int length){
    if (!isInit) return;
    SoundTouchUtils::getInstance()->putData(currentSpeedIndex,audio,length);
    int frameSize = DEFAULT_AUDIO_FRAME_SIZE_SINGLE * 2;
    memset(resultData,0x00,frameSize);
    while (true){
        int size = SoundTouchUtils::getInstance()->getData(currentSpeedIndex,&resultData,length);
        if(size <= 0) break;
        audioCallback(resultData,size);
    }
}

void SoundTouchHelper::destroy(){
    delete [] resultData;
    for (int i = 0; i < AUDIO_TRACK_TOTAL; ++i) {
        if (isInit) SoundTouchUtils::getInstance()->close(i);
    }
    isInit = false;
}
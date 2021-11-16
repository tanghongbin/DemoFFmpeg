//
// Created by Admin on 2021/10/15.
//

#ifndef DEMOFFMPEG_SOUNDTOUCHHELPER_H
#define DEMOFFMPEG_SOUNDTOUCHHELPER_H

#include <stdlib.h>
#include <utils/utils.h>

#include "utils/SoundTouchUtils.h"
#include "utils/SoundTouchEngine.h"
#include "encoder/hw/AudioConfiguration.h"
#include <map>
/***
 * 音频速率改变帮助类
 */
class SoundTouchHelper {
private:
    ReceiveAudioData audioCallback;
    SAMPLETYPE * resultData;
    int currentSpeedIndex;
    std::map<double,int> speedIndexMap;
    bool isInit;
public:
    SoundTouchHelper();
    void setAudioCallback(ReceiveAudioData audio);
    void init();
    void setSpeed(double speed);
    /***
     * 调整音频速率
     * @param audio
     * @param length
     */
    void adjustAudioData(uint8_t* audio,int length);
    void destroy();
};


#endif //DEMOFFMPEG_SOUNDTOUCHHELPER_H

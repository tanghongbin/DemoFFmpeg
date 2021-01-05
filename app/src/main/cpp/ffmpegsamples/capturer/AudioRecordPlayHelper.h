//
// Created by Admin on 2020/10/21.
//

#ifndef DEMOC_AUDIORECORDPLAYHELPER_H
#define DEMOC_AUDIORECORDPLAYHELPER_H

#include <encode/FFmpegEncodeAVToMp4.h>

#define SAMPLERATE 44100
#define CHANNELS 2
#define PERIOD_TIME 20 //ms
#define FRAME_SIZE SAMPLERATE*PERIOD_TIME/1000
//#define BUFFER_SIZE FRAME_SIZE*CHANNELS*2
#define BUFFER_SIZE 4096
#define TEST_CAPTURE_FILE_PATH "/storage/emulated/0/ffmpegtest/capture.pcm"

typedef void (*recordCall)(uint8_t* ,int);

class AudioRecordPlayHelper {

private:
    static AudioRecordPlayHelper* instance;
    volatile bool g_loop_exit = false;
    std::mutex mutex;
    std::condition_variable variable;


public:

    void startCapture(AbsEncodeAv *pMp4);

    void startCapture();

    void stopCapture();

    void startPlayBack();

    void notify_weak();

    int getBufferSize();

    void stopPlayBack();

    static AudioRecordPlayHelper* getInstance();

    static void destroyInstance();

};


#endif //DEMOC_AUDIORECORDPLAYHELPER_H

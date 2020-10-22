//
// Created by Admin on 2020/10/21.
//

#ifndef DEMOC_AUDIORECORDPLAYHELPER_H
#define DEMOC_AUDIORECORDPLAYHELPER_H

#define SAMPLERATE 44100
#define CHANNELS 1
#define PERIOD_TIME 20 //ms
#define FRAME_SIZE SAMPLERATE*PERIOD_TIME/1000
#define BUFFER_SIZE FRAME_SIZE*CHANNELS
#define TEST_CAPTURE_FILE_PATH "/sdcard/audio.pcm"

typedef void (*recordCall)(int,int);

class AudioRecordPlayHelper {

private:
    static AudioRecordPlayHelper* instance;
    volatile bool g_loop_exit = false;


public:

    void startCapture();

    void stopCapture();

    void startPlayBack();

    void stopPlayBack();

    static AudioRecordPlayHelper* getInstance();

    static void destroyInstance();

};


#endif //DEMOC_AUDIORECORDPLAYHELPER_H

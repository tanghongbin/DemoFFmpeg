//
// Created by Admin on 2020/10/21.
//

#ifndef DEMOC_OPENSL_IO_H
#define DEMOC_OPENSL_IO_H

// ========================录制和播放音频元数据pcm

#include <bits/pthread_types.h>
#include <SLES/OpenSLES.h>
#include <SLES/OpenSLES_Android.h>

typedef struct threadLock_ {
    pthread_mutex_t m;
    pthread_cond_t  c;
    unsigned char   s;
} threadLock;


typedef struct opensl_stream {

    // engine interfaces
    SLObjectItf engineObject;
    SLEngineItf engineEngine;

    // output mix interfaces
    SLObjectItf outputMixObject;

    // buffer queue player interfaces
    SLObjectItf bqPlayerObject;
    SLPlayItf bqPlayerPlay;
    SLAndroidSimpleBufferQueueItf bqPlayerBufferQueue;
    SLEffectSendItf bqPlayerEffectSend;

    // recorder interfaces
    SLObjectItf recorderObject;
    SLRecordItf recorderRecord;
    SLAndroidSimpleBufferQueueItf recorderBufferQueue;

    // buffer indexes
    int currentInputIndex;
    int currentOutputIndex;

    // current buffer half (0, 1)
    int currentOutputBuffer;
    int currentInputBuffer;

    // buffers
    uint8_t *outputBuffer[2];
    uint8_t *inputBuffer[2];

    // size of buffers
    int outBufSamples;
    int inBufSamples;

    // locks
    void*  inlock;
    void*  outlock;

    double time;
    int inchannels;
    int outchannels;
    int sr;

} OPENSL_STREAM;

class OpenSL_IO {

public:
    /*
  Open the audio device with a given sampling rate (sr), input and output channels and IO buffer size
  in frames. Returns a handle to the OpenSL stream
*/
    static OPENSL_STREAM* android_OpenAudioDevice(int sr, int inchannels, int outchannels, int bufferframes);

/*
  Close the audio device
*/
    static void android_CloseAudioDevice(OPENSL_STREAM *p);

/*
  Read a buffer from the OpenSL stream *p, of size samples. Returns the number of samples read.
*/
    static int android_AudioIn(OPENSL_STREAM *p, uint8_t *buffer,int size);

/*
  Write a buffer to the OpenSL stream *p, of size samples. Returns the number of samples written.
*/
    static int android_AudioOut(OPENSL_STREAM *p, uint8_t *buffer,int size);

    static int waitThreadLock(void *lock);

    static void* createThreadLock(void);

    static void notifyThreadLock(void *lock);

    static void destroyThreadLock(void *lock);

    static SLresult openSLCreateEngine(OPENSL_STREAM *p);

    static SLresult openSLPlayOpen(OPENSL_STREAM *p);

    static void bqRecorderCallback(SLAndroidSimpleBufferQueueItf bq, void *context);

    static void bqPlayerCallback(SLAndroidSimpleBufferQueueItf bq, void *context);

    static SLresult openSLRecOpen(OPENSL_STREAM *p);

    static void openSLDestroyEngine(OPENSL_STREAM *p);

/*
  Get the current IO block time in seconds
*/
    double android_GetTimestamp(OPENSL_STREAM *p);

};


#endif //DEMOC_OPENSL_IO_H

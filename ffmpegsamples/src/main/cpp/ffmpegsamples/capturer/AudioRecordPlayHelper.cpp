//
// Created by Admin on 2020/10/21.
//

#include <cstdio>
#include <utils.h>
#include <jni.h>
#include <OpenSL_IO.h>
#include <__mutex_base>
#include <libavutil/samplefmt.h>
#include <encode/FFmpegEncodeAudio.h>
#include <encode/AbsEncodeAV.h>
#include "AudioRecordPlayHelper.h"

AudioRecordPlayHelper *AudioRecordPlayHelper::instance = nullptr;

void AudioRecordPlayHelper::startCapture(AbsEncodeAv *encodeInstance) {

    OPENSL_STREAM *stream = OpenSL_IO::android_OpenAudioDevice(SAMPLERATE, CHANNELS, CHANNELS,
                                                               FRAME_SIZE);
    int samples;
    uint8_t buffer[BUFFER_SIZE];
    g_loop_exit = 0;
    while (!g_loop_exit) {
        std::unique_lock<std::mutex> lock(mutex);
        if (!encodeInstance->checkIsRecording()) {
            variable.wait(lock);
        }
        lock.unlock();
//        LOGCATE("i'm recording audio");
        samples = OpenSL_IO::android_AudioIn(stream, buffer, BUFFER_SIZE);
//        LOGCATE("capture audio data success :%d",samples);
        if (samples < 0) {
            LOGCATE("android_AudioIn failed !\n");
            break;
        }
//        call(buffer,samples);
        encodeInstance->consume_every_frame(buffer, samples, 1);
//        if (fwrite((unsigned char *) buffer, samples * sizeof(short), 1, fp) != 1) {
//            LOGCATE("failed to save captured data !\n ");
//            break;
//        }
//        LOGCATE("capture %d samples !\n", samples);
    }

    OpenSL_IO::android_CloseAudioDevice(stream);


    LOGCATE("nativeStartCapture completed !");
}


void AudioRecordPlayHelper::startCapture() {

    OPENSL_STREAM *stream = OpenSL_IO::android_OpenAudioDevice(SAMPLERATE, CHANNELS, CHANNELS,
                                                               FRAME_SIZE);
    LOGCATE("log capture size:%d",BUFFER_SIZE);
    int samples;
    uint8_t buffer[BUFFER_SIZE];
    g_loop_exit = 0;
    FFmpegEncodeAudio::getInstance()->init(".mp3");
    while (!g_loop_exit) {
//        LOGCATE("i'm recording audio");
        samples = OpenSL_IO::android_AudioIn(stream, buffer, BUFFER_SIZE);
//        LOGCATE("capture audio data success :%d",samples);
        if (samples < 0) {
            LOGCATE("android_AudioIn failed !\n");
            break;
        }
        FFmpegEncodeAudio::getInstance()->encodeAudioFrame(buffer,samples);
//        call(buffer,samples);
//        if (fwrite((unsigned char *) buffer, samples * sizeof(short), 1, fp) != 1) {
//            LOGCATE("failed to save captured data !\n ");
//            break;
//        }
//        LOGCATE("capture %d samples !\n", samples);
    }
    FFmpegEncodeAudio::getInstance()->unInit();
    FFmpegEncodeAudio::destroyInstance();
    OpenSL_IO::android_CloseAudioDevice(stream);


    LOGCATE("nativeStartCapture completed !");
}


void AudioRecordPlayHelper::notify_weak() {
    variable.notify_all();
}

void AudioRecordPlayHelper::stopCapture() {
    g_loop_exit = 1;
    variable.notify_all();
}

int AudioRecordPlayHelper::getBufferSize() {
//    return av_samples_get_buffer_size(NULL, pContext->channels, pFrame->nb_samples,
//                                      pContext->sample_fmt, 1);
    return 0;
}

void AudioRecordPlayHelper::startPlayBack() {
    FILE *fp = fopen(TEST_CAPTURE_FILE_PATH, "rb");
    if (fp == NULL) {
        LOGCATE("cannot open file (%s) !\n", TEST_CAPTURE_FILE_PATH);
        return;
    }

    OPENSL_STREAM *stream = OpenSL_IO::android_OpenAudioDevice(SAMPLERATE, CHANNELS, CHANNELS,
                                                               FRAME_SIZE);
    if (stream == NULL) {
        fclose(fp);
        LOGCATE("failed to open audio device ! \n");
        return;
    }

    int samples;
    uint8_t buffer[BUFFER_SIZE];
    g_loop_exit = 0;
    while (!g_loop_exit && !feof(fp)) {
        // todo there is something error
//        if (fread((uint8_t *) buffer, BUFFER_SIZE * 2, 1, fp) != 1) {
//            LOGCATE("failed to read data \n ");
//            break;
//        }
        samples = OpenSL_IO::android_AudioOut(stream, buffer, BUFFER_SIZE);
        if (samples < 0) {
            LOGCATE("android_AudioOut failed !\n");
        }
        LOGCATE("playback %d samples !\n", samples);
    }

    OpenSL_IO::android_CloseAudioDevice(stream);
    fclose(fp);

    LOGCATE("nativeStartPlayback completed !");
}

void AudioRecordPlayHelper::stopPlayBack() {
    g_loop_exit = true;
}

AudioRecordPlayHelper *AudioRecordPlayHelper::getInstance() {
    if (instance == nullptr) {
        instance = new AudioRecordPlayHelper;
    }
    return instance;
}

void AudioRecordPlayHelper::destroyInstance() {
    if (instance) {
        delete instance;
        instance = nullptr;
    }
}

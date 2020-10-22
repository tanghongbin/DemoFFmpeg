//
// Created by Admin on 2020/10/21.
//

#include <cstdio>
#include <utils.h>
#include <jni.h>
#include <OpenSL_IO.h>
#include <__mutex_base>
#include "AudioRecordPlayHelper.h"

AudioRecordPlayHelper *AudioRecordPlayHelper::instance = nullptr;

void AudioRecordPlayHelper::startCapture() {
    FILE *fp = fopen(TEST_CAPTURE_FILE_PATH, "wb");
    if (fp == NULL) {
        LOGCATE("cannot open file (%s)\n", TEST_CAPTURE_FILE_PATH);
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
    while (!g_loop_exit) {
        samples = OpenSL_IO::android_AudioIn(stream, buffer, BUFFER_SIZE);
        if (samples < 0) {
            LOGCATE("android_AudioIn failed !\n");
            break;
        }
        if (fwrite((unsigned char *) buffer, samples * sizeof(short), 1, fp) != 1) {
            LOGCATE("failed to save captured data !\n ");
            break;
        }
        LOGCATE("capture %d samples !\n", samples);
    }

    OpenSL_IO::android_CloseAudioDevice(stream);
    fclose(fp);

    LOGCATE("nativeStartCapture completed !");
}

void AudioRecordPlayHelper::stopCapture() {
    g_loop_exit = true;
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
    short buffer[BUFFER_SIZE];
    g_loop_exit = 0;
    while (!g_loop_exit && !feof(fp)) {
        if (fread((unsigned char *) buffer, BUFFER_SIZE * 2, 1, fp) != 1) {
            LOGCATE("failed to read data \n ");
            break;
        }
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

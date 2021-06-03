//
// Created by Admin on 2020/10/21.
//

#include <cstdio>
#include <jni.h>
#include <__mutex_base>
#include <libavutil/samplefmt.h>
#include <utils/AudioRecordPlayHelper.h>
#include <utils/OpenSL_IO.h>
#include <utils/utils.h>
#include <utils/CustomGLUtils.h>

AudioRecordPlayHelper *AudioRecordPlayHelper::instance = nullptr;

#define TEST_RECORD_FILE false

void AudioRecordPlayHelper::startCapture(RecordCall  encodeInstance) {

#if TEST_RECORD_FILE
    long long int startTime = GetSysCurrentTime();
    FILE *fp = fopen(TEST_CAPTURE_FILE_PATH, "wb");
    if (fp == NULL) {
        LOGCATE("cannot open file (%s) !\n", TEST_CAPTURE_FILE_PATH);
        return;
    }
#endif
    OPENSL_STREAM *stream = OpenSL_IO::android_OpenAudioDevice(SAMPLERATE, CHANNELS, CHANNELS,
                                                               FRAME_SIZE);
    int samples;
    uint8_t buffer[BUFFER_SIZE];
    g_loop_exit = 0;
    while (!g_loop_exit) {
        samples = OpenSL_IO::android_AudioIn(stream, buffer, BUFFER_SIZE);
        if (samples < 0) {
            LOGCATE("android_AudioIn failed !\n");
            break;
        }
#if TEST_RECORD_FILE
        if (GetSysCurrentTime() - startTime > 10 * 1000) {
            break;
        }
        LOGCATE("数量:%d",samples);
        if (fwrite((unsigned char *) buffer, BUFFER_SIZE, 1, fp) != 1) {
            LOGCATE("failed to save captured data !\n ");
            break;
        }
#else
            encodeInstance(buffer, samples);
#endif
//        LOGCATE("capture %d samples !\n", samples);
    }
    OpenSL_IO::android_CloseAudioDevice(stream);

#if TEST_RECORD_FILE
    fclose(fp);
#endif
    LOGCATE("nativeStartCapture completed !");
}

void AudioRecordPlayHelper::notify_weak() {
    variable.notify_all();
}

void AudioRecordPlayHelper::stopCapture() {
    g_loop_exit = true;
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
        if (fread((uint8_t *) buffer, BUFFER_SIZE, 1, fp) != 1) {
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

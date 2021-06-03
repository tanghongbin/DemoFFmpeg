//
// Created by Admin on 2021/5/31.
//

#ifndef DEMOFFMPEG_FFMPEGMEDIAMUXER_H
#define DEMOFFMPEG_FFMPEGMEDIAMUXER_H

#include <thread>
#include <utils/CustomSafeBlockQueue.h>
#include <render/BaseRender.h>
#include "AbsMediaMuxer.h"

extern "C" {
#include <libavformat/avformat.h>
};

typedef struct _audio_info{
    uint8_t* data;
    int nb_samples;
    void recycle(){
        nb_samples;
        data = nullptr;
        delete this;
    }
} AudioRecordItemInfo;

// a wrapper around a single output AVStream
typedef struct OutputStream {
    AVStream *st;
    AVCodecContext *enc;

    /* pts of the next frame that will be generated */
    int64_t next_pts;
    int samples_count;

    AVFrame *frame;
    AVFrame *tmp_frame;

    float t, tincr, tincr2;

    void * audioInfo;

    struct SwsContext *sws_ctx;
    struct SwrContext *swr_ctx;
} OutputStream;

class FFmpegMediaMuxer : public AbsMediaMuxer{
private:

    static FFmpegMediaMuxer* instance;
    std::thread * thread;
    std::thread * audioRecordThread;
    char mTargetFilePath[128];
    BaseRender* videoRender;
    static int StartMuxer(const char * fileName);
    static void startRecord(void * pVoid);
    static void receiveAudioBuffer(uint8_t* data,int nb_samples);
    FFmpegMediaMuxer(){
        thread = audioRecordThread = 0;
        videoRender = 0;
    }
public:
    CustomSafeQueue<AudioRecordItemInfo*> audioQueue;
    int init(const char * outFileName);
    void Destroy();
     static FFmpegMediaMuxer* getInstace(){
        if (instance == nullptr){
            instance = new FFmpegMediaMuxer;
        }
        return instance;
    }
    void test(int type);

    static void encodeMediaAV( AVFormatContext *oc, int encode_video, int encode_audio,
                  OutputStream &video_st,
                  OutputStream &audio_st);

    void OnSurfaceCreate();
    void OnSurfaceChanged(int width,int height);
    void OnCameraFrameDataValible(uint8_t* data);
    void OnDrawFrame();
};

#endif //DEMOFFMPEG_FFMPEGMEDIAMUXER_H

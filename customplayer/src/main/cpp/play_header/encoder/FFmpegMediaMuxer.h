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
    char mTargetFilePath[128];
    BaseVideoRender* videoRender;
    static int StartMuxer(const char * fileName);
    static void startRecord(void * pVoid);
    static void receiveAudioBuffer(uint8_t* data,int nb_samples);
    static void receivePixelData(int type,NativeOpenGLImage *pVoid);
    FFmpegMediaMuxer(){
        thread = 0;
        videoRender = 0;
        videoQueue.setMax(3);
        isDestroyed = false;
        cameraWidth = cameraHeight = 0;
    }
public:
    bool isDestroyed;
    std::mutex runningMutex;
    int cameraWidth,cameraHeight;
    CustomSafeQueue<AudioRecordItemInfo*> audioQueue;
    CustomSafeQueue<NativeOpenGLImage *> videoQueue;
    int init(const char * outFileName);
    void Destroy();
     static FFmpegMediaMuxer* getInstace(){
        if (instance == nullptr){
            instance = new FFmpegMediaMuxer;
        }
        return instance;
    }

    static void encodeMediaAV( AVFormatContext *oc, int encode_video, int encode_audio,
                  OutputStream &video_st,
                  OutputStream &audio_st);

    void OnSurfaceCreate();
    void OnSurfaceChanged(int width,int height);
    void OnCameraFrameDataValible(int type,NativeOpenGLImage* data);
    void OnDrawFrame();
    void OnAudioData(uint8_t *audioData, int length);
};

#endif //DEMOFFMPEG_FFMPEGMEDIAMUXER_H

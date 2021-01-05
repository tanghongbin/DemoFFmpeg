//
// Created by Admin on 2020/11/30.
//

#ifndef DEMOC_FFMPEGENCODEAVTOLIVERTMP_H
#define DEMOC_FFMPEGENCODEAVTOLIVERTMP_H


#include <cstdint>
#include <thread>

extern "C"{
#include <libswresample/swresample.h>
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
};

#define RTMP_PUSH_URL "rtmp://106.13.78.235:1935/demo/AndroidPush"

class FFmpegEncodeAVToLiveRtmp : public AbsEncodeAv{

private:

    // 视频参数
    AVCodec *codecV;
    AVCodecContext *codeCtxV = NULL;
    int ret;
    AVFrame *frameV;
    AVPacket *pktV;
    AVFormatContext* ofmtctx;
    AVStream* oStreamV;
    int bufferSize;
    int pts_frame_indexV = 1;
    int pts_frame_indexA = 1;

    // 音频参数
    AVCodec *codecA;
    AVCodecContext *codeCtxA;
    AVFrame *frameA;
    SwrContext *swr;
    AVPacket *packetA;
    AVStream *oStreamA;
    uint8_t *frame_bufferA;
    int64_t start_time;
    std::mutex mutex;

    int frameRate = 20;


    bool mHasInitSuccess = false;

    std::thread* audioCaptureThread;

    static FFmpegEncodeAVToLiveRtmp* instance;




    void configAudioEncodeParams(AVCodecContext *pContext, AVCodec *pCodec);

    void configFrameParams(AVFrame *pFrame, AVCodecContext *pContext);

    int check_sample_fmt(const AVCodec *codec, enum AVSampleFormat sample_fmt);

    int select_sample_rate(AVCodec *pCodec);

    uint64_t select_channel_layout(AVCodec *pCodec);

    int check_sample_fmt(AVCodec *pCodec, AVSampleFormat format);

    static void startCaptureAudio(FFmpegEncodeAVToLiveRtmp* instance);



public:
    bool isRecording = false;
    int mWindow_width;
    int mWindow_height;
    const char *out_file_name;

    static FFmpegEncodeAVToLiveRtmp* getInstance();

    static void destroyInstance();

    void initAvEncoder();

    void unInitAvEncoder();

    void startRecordMp4();

    void stopRecordMp4();

    // 1-音频，2-视频
    void encode_frame_Av(uint8_t* buffer, int size, int mediaType);

    int initEncodeVideo();

    int initEncodeAudio();

    void logPkt(AVFormatContext *pContext, AVPacket *pPacket, int i, AVRational rational);


    bool checkIsRecording();

    void consume_every_frame(uint8_t* buffer, int size, int mediaType);

};


#endif //DEMOC_FFMPEGENCODEAVTOLIVERTMP_H

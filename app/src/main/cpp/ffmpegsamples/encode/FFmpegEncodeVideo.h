//
// Created by Admin on 2020/10/26.
//

#ifndef DEMOC_FFMPEGENCODEVIDEO_H
#define DEMOC_FFMPEGENCODEVIDEO_H

#define OUT_PUT_VIDEO_PATH "/storage/emulated/0/ffmpegtest/encodeCameraVideo.h264"


class FFmpegEncodeVideo {

private:

    AVCodec *codec;
    AVCodecContext *codeCtx = NULL;
    int ret;
    AVFrame *frame;
    AVPacket *pkt;
    FILE *in_file;
    FILE *out_file;
    uint8_t* pic_buf;
    int mFrameIndex;
    bool mHasInitSuccess = false;
    static FFmpegEncodeVideo* instance;


public:
    int mWindow_width,mWindow_height;
    FFmpegEncodeVideo();

    ~FFmpegEncodeVideo();

    static FFmpegEncodeVideo* getInstance();

    static void destroyInstance();

    void init();

    void encodeVideoFrame(uint8_t* p);

    void unInit();

    void encode(AVCodecContext *pContext, AVFrame* frame, AVPacket *pPacket, FILE *pFile);

    void initOffcialDemo();

    void testReadFile();
};


#endif //DEMOC_FFMPEGENCODEVIDEO_H

//
// Created by Admin on 2020/10/26.
//

#ifndef DEMOC_FFMPEGENCODEVIDEO_H
#define DEMOC_FFMPEGENCODEVIDEO_H


class FFmpegEncodeVideo {

private:

    AVCodec *codec;
    AVCodecContext *codeCtx = NULL;
    int ret;
    AVFrame *frame;
    AVPacket *pkt;
    FILE *in_file;
    AVFormatContext* ofmtctx;
    AVStream* oStream;
    uint8_t* pic_buf;
    int bufferSize;
    int mFrameIndex;
    int pts_frame_index = 0;
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

};


#endif //DEMOC_FFMPEGENCODEVIDEO_H

//
// Created by Admin on 2020/10/26.
//

#ifndef DEMOC_FFMPEGENCODEVIDEO_H
#define DEMOC_FFMPEGENCODEVIDEO_H


class FFmpegEncodeVideo {

private:
    AVCodecContext* mCodeCtx;
    AVCodec* mCodec;
    AVFrame* mAvFrame;
    AVPacket* mAvPacket;
    FILE* mOutFile;
    int mFrameIndex;
    bool mHasInitSuccess = false;
    static FFmpegEncodeVideo* instance;

public:

    static FFmpegEncodeVideo* getInstance();

    static void destroyInstance();

    void init();

    void encodeVideoFrame(uint8_t* p);

    void unInit();

    void initOffcialDemo();

    void encode(AVCodecContext *pContext, AVFrame* frame, AVPacket *pPacket, FILE *pFile);
};


#endif //DEMOC_FFMPEGENCODEVIDEO_H

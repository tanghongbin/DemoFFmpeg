//
// Created by Admin on 2021/5/13.
//

#ifndef DEMOFFMPEG_BASEDECODER_H
#define DEMOFFMPEG_BASEDECODER_H

#include <cstdint>
#include <render/BaseRender.h>
extern "C" {
#include <libavformat/avformat.h>
};

class BaseDecoder{
private:
    static void createReadThread(BaseDecoder* baseDecoder);
    std::thread* readThread;
    const char * mUrl;
    std::mutex customMutex;
    std::condition_variable condition;
    bool isRunning = true;
    bool isStarted;
    void decodeLoop(AVFormatContext *pContext, AVCodecContext *pCodecContext, int i);
protected:
    // 1-音频，2-视频
    int appointMediaType;
protected:
    BaseRender* render;
public:
    PrepareCall call;
    void setMediaType(int mediaType) {
        appointMediaType = mediaType;
    };
    void Init(const char * url);
    void destroyThread();
    virtual void Destroy() {};
};

#endif //DEMOFFMPEG_BASEDECODER_H

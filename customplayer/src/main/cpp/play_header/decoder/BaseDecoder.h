//
// Created by Admin on 2021/5/13.
//

#ifndef DEMOFFMPEG_BASEDECODER_H
#define DEMOFFMPEG_BASEDECODER_H

#include <cstdint>
#include <render/BaseRender.h>
#include <render/AudioDataConverter.h>

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
    int mManualSeekPosition = -1;
    void decodeLoop(AVFormatContext *pContext, AVCodecContext *pCodecContext, int i);
    void OnDecodeReady(AVFormatContext *pContext, int i);
protected:
    // 1-音频，2-视频
    int appointMediaType;
protected:
    BaseRender* render;
    virtual BaseDataCoverter * createConverter() = 0;
public:
    PrepareCall call;
    void setMediaType(int mediaType) {
        appointMediaType = mediaType;
    };
    void Init(const char * url);
    void Destroy();
    void Start();
    void Stop();
    void ManualSeekPosition(int position);
};

#endif //DEMOFFMPEG_BASEDECODER_H

//
// Created by Admin on 2021/5/13.
//

#ifndef DEMOFFMPEG_BASEDECODER_H
#define DEMOFFMPEG_BASEDECODER_H

#include <cstdint>
#include <render/BaseRender.h>
#include <render/AudioDataConverter.h>
#include <render/VideoRender.h>

extern "C" {
#include <libavformat/avformat.h>
};

class BaseDecoder{
private:
    static void createReadThread(BaseDecoder* baseDecoder);
    std::thread* readThread;
    const char * mUrl;
    bool isRunning = true;
    bool isStarted;
    int mManualSeekPosition = -1;
    void decodeLoop(AVFormatContext *pContext, AVCodecContext *pCodecContext, int i,
                    BaseDecoder *pDecoder);
    void OnDecodeReady(AVFormatContext *pContext, int i);
protected:
    // 1-音频，2-视频
    int appointMediaType;
protected:
    BaseRender* render;
    virtual BaseDataCoverter * createConverter() = 0;
public:
    VideoRender* videoRender = 0;
    std::mutex customMutex;
    std::condition_variable condition;
    std::mutex createSurfaceMutex;
    std::condition_variable createSurfaceCondition;
    BaseDataCoverter* mDataConverter = 0;
    PrepareCall call;
    void setMediaType(int mediaType) {
        appointMediaType = mediaType;
    };
    void Init(const char * url);
    void Destroy();
    void Start();
    void Stop();
    void ManualSeekPosition(int position);
    BaseDecoder();
};

#endif //DEMOFFMPEG_BASEDECODER_H

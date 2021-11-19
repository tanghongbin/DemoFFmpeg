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
    bool isRunning;
    bool isStarted;
    int mManualSeekPosition;
    void decodeLoop(AVFormatContext *pContext, AVCodecContext *pCodecContext, int i,
                    BaseDecoder *pDecoder);
    void OnDecodeReady(AVFormatContext *pContext, int i);

protected:
    // 1-音频，2-视频
    int appointMediaType;
    int64_t currentAudioPts;

protected:
    virtual BaseDataCoverter * createConverter() = 0;
    virtual int64_t getCurrentAudioPts() {};
public:
    BaseDecoder();
    AVCodecContext* codeCtx;
    static void resolveConvertResult(void * decoder,void * data,int size);
    VideoRender* videoRender;
    OpenSLESRender* audioRender;
    std::mutex customMutex;
    std::condition_variable condition;
    std::mutex createSurfaceMutex;
    std::condition_variable createSurfaceCondition;
    BaseDataCoverter* mDataConverter;
    PrepareCall call;
    void setMediaType(int mediaType) {
        appointMediaType = mediaType;
    };
    void Init(const char * url);
    void Destroy();
    void Reset();
    void Start();
    void Stop();
    void ManualSeekPosition(int position);
    void Replay();
};

#endif //DEMOFFMPEG_BASEDECODER_H

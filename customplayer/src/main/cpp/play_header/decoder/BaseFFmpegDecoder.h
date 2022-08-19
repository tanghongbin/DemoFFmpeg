//
// Created by Admin on 2021/5/13.
//

#ifndef DEMOFFMPEG_BASEFFMPEGDECODER_H
#define DEMOFFMPEG_BASEFFMPEGDECODER_H

#include <cstdint>
#include <render/BaseRender.h>
#include <render/FFmpegAudioConverter.h>
#include <render/VideoRender.h>
#include <utils/TimeSyncHelper.h>
#include "BaseDecoder.h"

extern "C" {
#include <libavformat/avformat.h>
};

/***
 * ffmpeg 音视频解码
 */
class BaseFFmpegDecoder : public BaseDecoder{
private:
    void createReadThread(BaseFFmpegDecoder* baseDecoder);

    std::thread* readThread;
    const char * mUrl;
    bool isRunning;
    bool isStarted;
    int mManualSeekPosition;
    TimeSyncHelper* timeSyncHelper;
    void decodeLoop(AVFormatContext *pContext, AVCodecContext *pCodecContext, int i,
                    BaseFFmpegDecoder *pDecoder);
    void OnDecodeReady(AVFormatContext *pContext, int i);


protected:
    int64_t currentAudioPts;
    virtual BaseDataCoverter * createConverter() = 0;
    virtual int64_t getCurrentAudioPtsUs() {};
public:
    BaseFFmpegDecoder();
    AVCodecContext* codeCtx;
    static void resolveConvertResult(void * decoder,void * data,int size);
    std::mutex customMutex;
    std::condition_variable condition;
    std::mutex createSurfaceMutex;
    std::condition_variable createSurfaceCondition;
    BaseDataCoverter* mDataConverter;
    PrepareCall call;
    void Init(const char * url);
    void Destroy();
    void Reset();
    void Start();
    void Stop();
    void ManualSeekPosition(int position);
    void Replay();
};

#endif //DEMOFFMPEG_BASEFFMPEGDECODER_H

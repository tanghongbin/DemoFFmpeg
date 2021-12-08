//
// Created by Admin on 2021/11/22.
//

#ifndef DEMOFFMPEG_BASEHWDECODER_H
#define DEMOFFMPEG_BASEHWDECODER_H


#include <media/NdkMediaCodec.h>
#include <media/NdkMediaExtractor.h>
#include <media/NdkImageReader.h>
#include <cstdlib>
#include <stdlib.h>
#include <thread>
#include <utils/utils.h>
#include <encoder/hw/AudioConfiguration.h>
#include <decoder/BaseDecoder.h>
#include <utils/TimeSyncHelper.h>
#include <utils/Callback.h>

/***
 * 硬解码 基类
 */
class BaseHwDecoder : public BaseDecoder{
private:
    char mUrl[1024];
    void createDecoderThread(const char * url);
    void renderAv(int type,AMediaCodecBufferInfo* bufferInfo,uint8_t* data);
    std::thread* decodeThread;
    bool isFinished;
    bool isPaused;
    std::mutex mPauseMutex;
    std::condition_variable mPauseCondition;
    int seekProgress;
    int64_t currentPtsUs;
    PrepareCall readyCall;
    bool isInit;
    int mWindowWidth,mWindowHeight,oreration;
    uint8_t * i420dst;
    AMediaExtractor *mMediaExtractor;
    AMediaCodec *mMediaCodec;
    TimeSyncHelper* timeSyncHelper;
    void OnSizeReady();
    bool isNeedRender; // 是否需要渲染，通常只用作解码使用
    OutputDataListener outputDataListener;
    bool isNeedPauseWhenFinished;
    Callback* onCompleteCall;
    VideoConfigInfo* videoConfigInfo;
protected:
    std::mutex audioPtsMutex;
    BaseDataCoverter * createConverter() {};
public:
    int mVideoHeight,mVideoWidth;
    void setOutputDataListener(OutputDataListener listener){
        this->outputDataListener = listener;
    }
    void setOnCompleteCall(Callback* call) {
        this->onCompleteCall = call;
    }
    virtual void setMediaType(int mediaType) {
        this->appointMediaType = mediaType;
    }
    VideoConfigInfo* getVideoConfigInfo(){
        return videoConfigInfo;
    }
    std::mutex mCreateSurfaceMutex;
    std::condition_variable mSurfaceCondition;
    void setReadyCall(PrepareCall call);
    BaseHwDecoder();
    virtual char * getDecodeTypeStr() = 0;
    /***
     * 毫秒
     * @return
     */
    int64_t getCurrentAudioPtsUs();
    void Init(const char * url) ;
    void Start();
    void Stop();
    void Destroy();
    void ManualSeekPosition(int position);
    void Reset();
    void Replay();
    void OnSurfaceChanged(int oreration,int width,int height);
    void setIfNeedRender(bool needRender);
    void setIfNeedPauseWhenFinished(bool needRender);

};

#endif //DEMOFFMPEG_BASEHWDECODER_H

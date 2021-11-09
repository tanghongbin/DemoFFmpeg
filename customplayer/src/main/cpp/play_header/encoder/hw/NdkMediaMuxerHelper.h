//
// Created by Admin on 2021/7/15.
//

#ifndef DEMOFFMPEG_NDKMEDIAMUXERHELPER_H
#define DEMOFFMPEG_NDKMEDIAMUXERHELPER_H


#define MUX_TYPE_AV_ALL 1
#define MUX_TYPE_A_OR_V 2

#include <mutex>
extern "C" {
#include <media/NdkMediaMuxer.h>
}

/***
 * 音视频流合并器
 */
class NdkMediaMuxerHelper{
private:
    static NdkMediaMuxerHelper* instance;
    AMediaMuxer* mMuxer;
    int count;
    bool isStart;
    std::mutex mMutex;
    FILE * mFile;
    int muxType; // 1-音频和视频，2-音频或视频
public:
    NdkMediaMuxerHelper(){
        mFile = 0;
        mMuxer = 0;
        count = 0;
        isStart = false;
        muxType = 1;
    }
    NdkMediaMuxerHelper* getInstance(){
        if (instance == nullptr){
            instance = new NdkMediaMuxerHelper;
        }
        return instance;
    }
    void setMuxType(int type){
        muxType = type;
    }
    void init(const char * path);
    void writeSampleData(int trackIndex,uint8_t* data,AMediaCodecBufferInfo* info);
    int addTrack(AMediaFormat* mediaFormat);
    void start();
    void stop();
    void destroy();
};

#endif //DEMOFFMPEG_NDKMEDIAMUXERHELPER_H

//
// Created by Admin on 2021/7/15.
//

#ifndef DEMOFFMPEG_NDKMEDIAMUXERHELPER_H
#define DEMOFFMPEG_NDKMEDIAMUXERHELPER_H


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
public:
    NdkMediaMuxerHelper(){
        mMuxer = 0;
        count = 0;
        isStart = false;
    }
    NdkMediaMuxerHelper* getInstance(){
        if (instance == nullptr){
            instance = new NdkMediaMuxerHelper;
        }
        return instance;
    }
    void init(const char * path);
    void writeSampleData(int trackIndex,uint8_t* data,AMediaCodecBufferInfo* info);
    int addTrack(AMediaFormat* mediaFormat);
    void start();
    void stop();
    void destroy();
};

#endif //DEMOFFMPEG_NDKMEDIAMUXERHELPER_H

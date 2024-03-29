//
// Created by Admin on 2021/7/15.
//

#include <encoder/hw/NdkMediaMuxerHelper.h>
#include <string>
#include <utils/CustomGLUtils.h>

NdkMediaMuxerHelper* NdkMediaMuxerHelper::instance = nullptr;

void  NdkMediaMuxerHelper::init(const char * path){
    LOGCATE("打印合成地址:%s",path);
    mFile = fopen(path,"wb+");
    mMuxer =  AMediaMuxer_new(fileno(mFile),AMEDIAMUXER_OUTPUT_FORMAT_MPEG_4);
}

void NdkMediaMuxerHelper::writeSampleData(int trackIndex,uint8_t* data,AMediaCodecBufferInfo* info){
//    LOGCATE("开始写入sample数据:%d",trackIndex);
    std::lock_guard<std::mutex> lockGuard(mMutex);
    AMediaMuxer_writeSampleData(mMuxer,trackIndex,data,info);
}

int NdkMediaMuxerHelper::addTrack(AMediaFormat* mediaFormat){
    std::lock_guard<std::mutex> lockGuard(mMutex);
    count++;
    return AMediaMuxer_addTrack(mMuxer,mediaFormat);
}

void NdkMediaMuxerHelper::start(){
    LOGCATE("硬编码准备开始录制了");
    std::lock_guard<std::mutex> lockGuard(mMutex);
    if (isStart) return;
    if (muxType == MUX_TYPE_AV_ALL && count < 2) return;
    if (muxType == MUX_TYPE_A_OR_V && count < 1) return;
    AMediaMuxer_start(mMuxer);
    isStart = true;
}

void NdkMediaMuxerHelper::stop(){
    if (!isStart) return;
    std::lock_guard<std::mutex> lockGuard(mMutex);
    isStart = false;
    if (mMuxer) AMediaMuxer_stop(mMuxer);
    LOGCATE("NdkMediaMuxerHelper has stop");
}

void NdkMediaMuxerHelper::destroy(){
    std::lock_guard<std::mutex> lockGuard(mMutex);
    if (mMuxer) AMediaMuxer_delete(mMuxer);
    if (mFile) fclose(mFile);
    instance = 0;
}
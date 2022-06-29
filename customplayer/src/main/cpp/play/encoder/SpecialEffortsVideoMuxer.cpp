//
// Created by Admin on 2021/5/31.
//



#include <encoder/SpecialEffortsVideoMuxer.h>
#include <utils/utils.h>
#include <utils/CustomGLUtils.h>
#include <render/VideoRender.h>
#include <libyuv.h>
#include <libyuv/scale.h>
#include <libyuv/convert.h>
#include <libyuv/rotate.h>
#include <render/VideoFboRender.h>
#include <utils/SoundTouchUtils.h>
#include <encoder/OutputDisplayHelper.h>

/**************************************************************/
/* media file output */

SpecialEffortsVideoMuxer* SpecialEffortsVideoMuxer::instance = nullptr;


int SpecialEffortsVideoMuxer::init(const char * fileName){
    // delete folder
    mTargetPathV = const_cast<char *>(getRandomStr("specialEfforts-", ".mp4", "temp/"));
    createFolderIfNotExist(mTargetPathV);
    mMuxerHelper = new NdkMediaMuxerHelper;
    mMuxerHelper->setMuxType(MUX_TYPE_A_OR_V);
    mMuxerHelper->init(mTargetPathV);
    mediaCodecV = new MediaCodecVideo;
    mediaCodecV->setSpeed(speed);
    mediaCodecV->startEncode();
    mediaCodecV->setOutputDataListener(SpecialEffortsVideoMuxer::receiveCodecFmtChanged,SpecialEffortsVideoMuxer::receiveMediaCodecData);
    isStarted = true;
    return 0;
}

void SpecialEffortsVideoMuxer::receiveMediaCodecData(int type,AMediaCodecBufferInfo * bufferInfo, uint8_t* srcData){
    if (type == 2) getInstance()->mMuxerHelper->writeSampleData(getInstance() -> videoTrackIndex,srcData,bufferInfo);
}

void SpecialEffortsVideoMuxer::receiveCodecFmtChanged(int type,AMediaFormat* mediaFormat){
    if( type == 1) {
        // 音频
    } else {
        // 视频
        getInstance() -> videoTrackIndex = getInstance()->mMuxerHelper->addTrack(mediaFormat);
    }
    getInstance()->mMuxerHelper->start();
}


void SpecialEffortsVideoMuxer::receiveOriginalAvData(int type,void * pVoid,int size) { 
    if (type == 2){
        if (getInstance() -> mediaCodecV) {
            getInstance() -> mediaCodecV -> putData(static_cast<NativeOpenGLImage *>(pVoid));
        } else {
            auto *data = static_cast<NativeOpenGLImage *>(pVoid);
            NativeOpenGLImageUtil::FreeNativeImage(data);
            delete data;
        }
    } else {
        LOGCATE("receiveOriginalAvData unsupport type:%d",type);
    }
}

void SpecialEffortsVideoMuxer::Stop() {
    if (!isStarted) return;
    // 清空所有缓存音频
//    OutputDisplayHelper::getInstance()->setOutputListener(nullptr);
    std::unique_lock<std::mutex> uniqueLock(runningMutex,std::defer_lock);
    uniqueLock.lock();
    isDestroyed = true;
    uniqueLock.unlock();

    mediaCodecV->destroy();

    int64_t totalDuration = mediaCodecV->getTotalTime();
    delete mediaCodecV;
    mMuxerHelper->stop();
    mMuxerHelper->destroy();
    delete mMuxerHelper;
    isStarted = false;
    LOGCATE("mediacodec has already stopped video duration:%lld",totalDuration);
}

void SpecialEffortsVideoMuxer::setVideoConfigInfo(VideoConfigInfo * configInfo){
    if (mediaCodecV) {
        mediaCodecV->setVideoConfigInfo(configInfo);
    }
}

void SpecialEffortsVideoMuxer::Destroy(){
    if (isStarted) {
        Stop();
    }
    delete instance;
    instance = 0;
}

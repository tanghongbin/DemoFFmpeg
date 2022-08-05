//
// Created by Admin on 2021/5/31.
//



#include <encoder/HwMediaMuxer.h>
#include <utils/utils.h>
#include <utils/CustomGLUtils.h>
#include <render/VideoRender.h>
#include <libyuv.h>
#include <libyuv/scale.h>
#include <libyuv/convert.h>
#include <libyuv/rotate.h>
#include <render/VideoFboRender.h>
#include <utils/SoundTouchUtils.h>


HwMediaMuxer* HwMediaMuxer::instance = nullptr;

/**************************************************************/
/* media file output */

int HwMediaMuxer::init(const char * fileName){
    strcpy(mTargetFilePath,fileName);
    LOGCATE("打印地址:%s",fileName);
    createFolderIfNotExist(mTargetFilePath);
    mMuxerHelper = new NdkMediaMuxerHelper;
    mMuxerHelper->init(mTargetFilePath);
    mediaCodecA = new MediaCodecAudio;
    mediaCodecA->startEncode();
    mediaCodecA->setOutputDataListener(HwMediaMuxer::receiveCodecFmtChanged,HwMediaMuxer::receiveMediaCodecData);
    soundTouchHelper = new SoundTouchHelper;
    soundTouchHelper->setSpeed(speed);
    soundTouchHelper->init();
    soundTouchHelper->setAudioCallback(reinterpret_cast<ReceiveAudioData>(HwMediaMuxer::receiveSoundTouchData));
    mediaCodecV = new MediaCodecVideo;
    mediaCodecV->setSpeed(speed);
    mediaCodecV->startEncode();
    mediaCodecV->setOutputDataListener(HwMediaMuxer::receiveCodecFmtChanged,HwMediaMuxer::receiveMediaCodecData);
    isStarted = true;
    return 0;
}


void HwMediaMuxer::receivePixelData(int type,NativeOpenGLImage *srcData){
    // glreadypixel 后转换成720分辨率的420p的画面,可以直接使用
    if (getInstace()->mediaCodecV) {
        getInstace()->mediaCodecV->putData(srcData);
    }
}

void HwMediaMuxer::receiveSoundTouchData(short * srcData,int size){
    std::unique_lock<std::mutex> uniqueLock(HwMediaMuxer::getInstace() -> runningMutex,std::defer_lock);
    uniqueLock.lock();
    if (HwMediaMuxer::getInstace()->isDestroyed || !HwMediaMuxer::getInstace()->isStarted){
        uniqueLock.unlock();
        return;
    }
    uniqueLock.unlock();

    uint8_t resultData[size];
    memcpy(resultData,srcData,size);
    if (getInstace()->mediaCodecA){
        getInstace()->mediaCodecA->putData(resultData,size);
    }
}

void HwMediaMuxer::receiveMediaCodecData(int type,AMediaCodecBufferInfo * bufferInfo, uint8_t* data){
    if( type == 1) {
        // 音频
        getInstace()->mMuxerHelper->writeSampleData(getInstace() -> audioTrackIndex,data,bufferInfo);
    } else {
        // 视频
        getInstace()->mMuxerHelper->writeSampleData(getInstace() -> videoTrackIndex,data,bufferInfo);
    }
}

void HwMediaMuxer::getInAvDataFunc(ReceiveAvOriginalData* getParams) {
    getParams->audioCall = reinterpret_cast<ReceiveAudioData>(HwMediaMuxer::receiveSoundTouchData);
    getParams->videoCall = HwMediaMuxer::receivePixelData;
}

void HwMediaMuxer::receiveCodecFmtChanged(int type,AMediaFormat* mediaFormat){
    if( type == 1) {
        // 音频
        getInstace() -> audioTrackIndex = getInstace()->mMuxerHelper->addTrack(mediaFormat);
    } else {
        // 视频
        getInstace() -> videoTrackIndex = getInstace()->mMuxerHelper->addTrack(mediaFormat);
    }
    getInstace()->mMuxerHelper->start();
}

/***
 * 原生数据，要经过soundtouch的处理
 * @param audioData
 * @param length
 */
void HwMediaMuxer::OnAudioData(uint8_t *audioData, jint length) {
    std::unique_lock<std::mutex> uniqueLock(HwMediaMuxer::getInstace() -> runningMutex,std::defer_lock);
    uniqueLock.lock();
    if (isDestroyed  || !isStarted){
        uniqueLock.unlock();
        return;
    }
    uniqueLock.unlock();

    if (speed != 1.0) {
        soundTouchHelper->adjustAudioData(audioData,length);
    } else {
        if (getInstace()->mediaCodecA){
            getInstace()->mediaCodecA->putData(audioData,length);
        }
    }
    LOGCATE("log mediacodea:%p",mediaCodecA);
}


void HwMediaMuxer::Destroy(){
    // 清空所有缓存音频
    std::unique_lock<std::mutex> uniqueLock(runningMutex,std::defer_lock);
    uniqueLock.lock();
    isDestroyed = true;
    uniqueLock.unlock();
    if (thread) {
        thread -> join();
        delete thread;
    }
    if (mediaCodecA) {
        mediaCodecA->destroy();
        delete mediaCodecA;
    }
    if (mediaCodecV) {
        mediaCodecV->destroy();
        delete mediaCodecV;
    }
    if (soundTouchHelper) {
        soundTouchHelper->destroy();
        delete soundTouchHelper;
    }
    if (mMuxerHelper) {
        mMuxerHelper->stop();
        mMuxerHelper->destroy();
        delete mMuxerHelper;
    }
    instance = 0;
}
//
// Created by Admin on 2021/5/31.
//



#include <encoder/ShortVideoMuxer.h>
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


ShortVideoMuxer* ShortVideoMuxer::instance = nullptr;

/**************************************************************/
/* media file output */

int ShortVideoMuxer::init(const char * fileName){
    strcpy(mTargetFilePath,fileName);
    LOGCATE("打印地址:%s",fileName);
    createFolderIfNotExist(mTargetFilePath);
    // delete folder
    mTargetPathA = const_cast<char *>(getRandomStr("encodea-", ".aac", "temp/"));
    createFolderIfNotExist(mTargetPathA);
    mTargetFileA = fopen(mTargetPathA,"wb+");
    mTargetPathV = const_cast<char *>(getRandomStr("encodev-", ".mp4", "temp/"));
    createFolderIfNotExist(mTargetPathV);
    mMuxerHelper = new NdkMediaMuxerHelper;
    mMuxerHelper->setMuxType(MUX_TYPE_A_OR_V);
    mMuxerHelper->init(mTargetPathV);
    mediaCodecA = new MediaCodecAudio;
    mediaCodecA->startEncode();
    mediaCodecA->setOutputDataListener(ShortVideoMuxer::receiveCodecFmtChanged,ShortVideoMuxer::receiveMediaCodecData);
    mediaCodecV = new MediaCodecVideo;
    mediaCodecV->setSpeed(speed);
    mediaCodecV->startEncode();
    mediaCodecV->setOutputDataListener(ShortVideoMuxer::receiveCodecFmtChanged,ShortVideoMuxer::receiveMediaCodecData);
    isStarted = true;
//    OutputDisplayHelper::getInstance()->setOutputListener(ShortVideoMuxer::receiveOriginalAvData);
    return 0;
}

void ShortVideoMuxer::receiveMediaCodecData(int type,AMediaCodecBufferInfo * bufferInfo, uint8_t* srcData){
    if( type == 1) {
        // 音频
        int bufferSize = bufferInfo->size + 7;
        auto* resultData = new uint8_t [bufferSize];
        memset(resultData,0x00,bufferSize);
        getInstance() -> addAtdsHeader(resultData,bufferSize);
        memcpy(resultData + 7,srcData,bufferInfo->size);
        fwrite(resultData,1,bufferSize,getInstance() -> mTargetFileA);
        delete [] resultData;
//        getInstance()->mTargetFileA -> wri
//        getInstance()->mMuxerHelper->writeSampleData(getInstance() -> audioTrackIndex,data,bufferInfo);

    } else {
        // 视频
        getInstance()->mMuxerHelper->writeSampleData(getInstance() -> videoTrackIndex,srcData,bufferInfo);
    }
}

void ShortVideoMuxer::receiveCodecFmtChanged(int type,AMediaFormat* mediaFormat){
    if( type == 1) {
        // 音频
    } else {
        // 视频
        getInstance() -> videoTrackIndex = getInstance()->mMuxerHelper->addTrack(mediaFormat);
    }
    getInstance()->mMuxerHelper->start();
}


void ShortVideoMuxer::receiveOriginalAvData(int type,void * pVoid,int size) { 
    if (type == 1) {
        if (getInstance() -> mediaCodecA) {
            getInstance() -> mediaCodecA -> putData(static_cast<uint8_t *>(pVoid), size);
        }
    } else if (type == 2){
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

void ShortVideoMuxer::Stop() {
    if (!isStarted) return;
    // 清空所有缓存音频
//    OutputDisplayHelper::getInstance()->setOutputListener(nullptr);
    std::unique_lock<std::mutex> uniqueLock(runningMutex,std::defer_lock);
    uniqueLock.lock();
    isDestroyed = true;
    uniqueLock.unlock();
    if (thread) {
        thread -> join();
        delete thread;
    }
    mediaCodecA->destroy();
    delete mediaCodecA;
    mediaCodecV->destroy();

    int64_t totalDuration = mediaCodecV->getTotalTime();
    delete mediaCodecV;
    mMuxerHelper->stop();
    mMuxerHelper->destroy();
    delete mMuxerHelper;
    if (mTargetFileA) {
        fclose(mTargetFileA);
//        remove(mTargetPathA);
//        remove(mTargetPathV);
    }
    char resultStr[1028];
    sprintf(resultStr,"%lld %s %s",totalDuration,mTargetPathA,mTargetPathV);
    MsgLoopHelper::sendMsg(Message::obtain(JNI_COMMUNICATE_TYPE_MERGE_AV,1,1,resultStr));
    isStarted = false;
    LOGCATE("mediacodec has already stopped video duration:%lld",totalDuration);
}

void ShortVideoMuxer::Destroy(){
    if (isStarted) {
        Stop();
    }
    delete instance;
    instance = 0;
}

void ShortVideoMuxer::addAtdsHeader(uint8_t * data, int32_t length) {
    int profile = 2; // AAC LC
    int freqIdx = 4; // 44.1KHz
    int chanCfg = 2; // CPE
    // fill in A D T S data
    data[0] = (uint8_t) 0xFF;
    data[1] = (uint8_t) 0xF9;
    data[2] = (uint8_t) (((profile - 1) << 6) + (freqIdx << 2) + (chanCfg >> 2));
    data[3] = (uint8_t) (((chanCfg & 3) << 6) + (length >> 11));
    data[4] = (uint8_t) ((length & 0x7FF) >> 3);
    data[5] = (uint8_t) (((length & 7) << 5) + 0x1F);
    data[6] = (uint8_t) 0xFC;
}

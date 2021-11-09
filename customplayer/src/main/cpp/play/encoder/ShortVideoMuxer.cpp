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
    soundTouchHelper = new SoundTouchHelper;
    soundTouchHelper->setSpeed(speed);
    soundTouchHelper->init();
    soundTouchHelper->setAudioCallback(reinterpret_cast<ReceiveAudioData>(ShortVideoMuxer::receiveSoundTouchData));
    mediaCodecV = new MediaCodecVideo;
    mediaCodecV->setSpeed(speed);
    mediaCodecV->startEncode();
    mediaCodecV->setOutputDataListener(ShortVideoMuxer::receiveCodecFmtChanged,ShortVideoMuxer::receiveMediaCodecData);
    isStarted = true;
    return 0;
}


void ShortVideoMuxer::receivePixelData(int type,NativeOpenGLImage *pVoid){
    ShortVideoMuxer::getInstance()->OnCameraFrameDataValible(type, pVoid);
}

void ShortVideoMuxer::receiveSoundTouchData(short * srcData,int size){
    std::unique_lock<std::mutex> uniqueLock(ShortVideoMuxer::getInstance() -> runningMutex,std::defer_lock);
    uniqueLock.lock();
    if (ShortVideoMuxer::getInstance()->isDestroyed || !ShortVideoMuxer::getInstance()->isStarted){
        uniqueLock.unlock();
        return;
    }
    uniqueLock.unlock();

    uint8_t resultData[size];
    memcpy(resultData,srcData,size);
    if (getInstance()->mediaCodecA){
        getInstance()->mediaCodecA->putData(resultData,size);
    }
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

void ShortVideoMuxer::OnSurfaceCreate() {
    videoRender = new VideoFboRender;
    videoRender->readPixelCall = ShortVideoMuxer::receivePixelData;
    videoRender->Init();
}
void ShortVideoMuxer::OnSurfaceChanged(int width,int height) {
    videoRender->OnSurfaceChanged(width,height);
}
void ShortVideoMuxer::OnCameraFrameDataValible(int type,NativeOpenGLImage * srcData) {
    // 4-glreadPixel 读出来的数据已经转换成720分辨率的420p的数据了,可以直接编码
    std::unique_lock<std::mutex> uniqueLock(ShortVideoMuxer::getInstance() -> runningMutex,std::defer_lock);
    uniqueLock.lock();
    if (isDestroyed){
        uniqueLock.unlock();
        return;
    }
    uniqueLock.unlock();

    if (type == 1) {
        // 1-java camera nv21 转换成i420直接编码成mp4  - 适用于surfaceview
        if (isDestroyed || cameraWidth == 0 || cameraHeight == 0) return;
//        int bufferSize = cameraWidth * cameraHeight * 3 / 2;
//        uint8_t * i420Dst = new uint8_t [bufferSize];
//        memset(i420Dst,0x00,bufferSize);
//        yuvNv21To420p(srcData,i420Dst,cameraWidth,cameraHeight,libyuv::kRotate90);
//        uint8_t * lastData = videoQueue.pushLast(i420Dst);
//        if (lastData) delete [] lastData;
    } else if (type == 2) {
        // camera-yuv420p 数据，直接渲染成Opengles
        int localWidth = 1280;
        int localHeight = 720;
        uint8_t * i420srcData = srcData->ppPlane[0];
        uint8_t * i420dstData = new uint8_t [localWidth * localHeight * 3/2];
        yuvI420Rotate(i420srcData, i420dstData, localWidth, localHeight, libyuv::kRotate270, true);
        srcData -> width = localHeight;
        srcData -> height = localWidth;
        srcData -> format = IMAGE_FORMAT_I420;
        srcData -> ppPlane[0] = i420dstData;
        srcData -> ppPlane[1] = i420dstData + localWidth * localHeight;
        srcData -> ppPlane[2] = i420dstData + localWidth * localHeight * 5 / 4;
        srcData -> pLineSize[0] = localHeight;
        srcData -> pLineSize[1] = localHeight >> 1;
        srcData -> pLineSize[2] = localHeight >> 1;
        videoRender->copyImage(srcData);
        delete [] i420dstData;
//        LOGCATE("log width:%d height:%d imagewidth:%d imageheight:%d",localWidth,localHeight,openGlImage.width,openGlImage.height);
    } else if (type == 3) {
        // 3-rgba放进队列
        if (mediaCodecV) {
            mediaCodecV->putData(srcData);
        } else {
            NativeOpenGLImageUtil::FreeNativeImage(srcData);
            delete srcData;
        }
    } else if (type == 4) {
        // glreadypixel 后转换成720分辨率的420p的画面,可以直接使用
        if (mediaCodecV) {
            mediaCodecV->putData(srcData);
        } else {
            NativeOpenGLImageUtil::FreeNativeImage(srcData);
            delete srcData;
        }
    } else {
        LOGCATE("OnCameraFrameDataValible not support type:%d",type);
    }


}

/***
 * 原生数据，要经过soundtouch的处理
 * @param audioData
 * @param length
 */
void ShortVideoMuxer::OnAudioData(uint8_t *audioData, jint length) {
    std::unique_lock<std::mutex> uniqueLock(ShortVideoMuxer::getInstance() -> runningMutex,std::defer_lock);
    uniqueLock.lock();
    if (isDestroyed  || !isStarted){
        uniqueLock.unlock();
        return;
    }
    uniqueLock.unlock();

    if (speed != 1.0) {
        soundTouchHelper->adjustAudioData(audioData,length);
    } else {
        if (getInstance()->mediaCodecA){
            getInstance()->mediaCodecA->putData(audioData,length);
        }
    }
}

void ShortVideoMuxer::OnDrawFrame(){
    if (videoRender) videoRender->DrawFrame();
}

void ShortVideoMuxer::Destroy(){
    // 清空所有缓存音频
    std::unique_lock<std::mutex> uniqueLock(runningMutex,std::defer_lock);
    uniqueLock.lock();
    isDestroyed = true;
    uniqueLock.unlock();
    if (thread) {
        thread -> join();
        delete thread;
    }
    if(videoRender){
        videoRender->Destroy();
        delete videoRender;
        videoRender = 0;
    }
    mediaCodecA->destroy();
    delete mediaCodecA;
    mediaCodecV->destroy();
    soundTouchHelper->destroy();

    int64_t totalDuration = mediaCodecV->getTotalTime();

    delete soundTouchHelper;
    delete mediaCodecV;
    mMuxerHelper->stop();
    mMuxerHelper->destroy();
    if (mTargetFileA) {
        // todo 待会儿移除
        fclose(mTargetFileA);
//        remove(mTargetPathA);
//        remove(mTargetPathV);
    }
    char resultStr[1028];
    sprintf(resultStr,"%lld %s %s",totalDuration,mTargetPathA,mTargetPathV);
    MsgLoopHelper::sendMsg(Message::obtain(JNI_COMMUNICATE_TYPE_MERGE_AV,1,1,resultStr));
    delete mMuxerHelper;
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

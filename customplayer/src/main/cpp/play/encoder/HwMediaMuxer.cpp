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


HwMediaMuxer* HwMediaMuxer::instance = nullptr;

/**************************************************************/
/* media file output */

int HwMediaMuxer::init(const char * fileName){
    strcpy(mTargetFilePath,fileName);
    LOGCATE("打印地址:%s",fileName);
    access(fileName,0);
    mMuxerHelper = new NdkMediaMuxerHelper;
    mMuxerHelper->init(mTargetFilePath);
    mediaCodecA = new MediaCodecAudio;
    mediaCodecA->startEncode();
    mediaCodecA->setOutputDataListener(HwMediaMuxer::receiveCodecFmtChanged,HwMediaMuxer::receiveMediaCodecData);
    mediaCodecV = new MediaCodecVideo;
    mediaCodecV->startEncode();
    mediaCodecV->setOutputDataListener(HwMediaMuxer::receiveCodecFmtChanged,HwMediaMuxer::receiveMediaCodecData);
    return 0;
}


void HwMediaMuxer::receivePixelData(int type,NativeOpenGLImage *pVoid){
    HwMediaMuxer::getInstace()->OnCameraFrameDataValible(type, pVoid);
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

void HwMediaMuxer::OnSurfaceCreate() {
    videoRender = new VideoFboRender;
    videoRender->readPixelCall = HwMediaMuxer::receivePixelData;
    videoRender->Init();
}
void HwMediaMuxer::OnSurfaceChanged(int width,int height) {
    videoRender->OnSurfaceChanged(width,height);
}
void HwMediaMuxer::OnCameraFrameDataValible(int type,NativeOpenGLImage * srcData) {
    // 4-glreadPixel 读出来的数据已经转换成720分辨率的420p的数据了,可以直接编码
    std::unique_lock<std::mutex> uniqueLock(HwMediaMuxer::getInstace() -> runningMutex,std::defer_lock);
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
        if (mediaCodecV) mediaCodecV->putData(srcData);
    } else if (type == 4) {
        // glreadypixel 后转换成720分辨率的420p的画面,可以直接使用
        if (mediaCodecV) mediaCodecV->putData(srcData);
    } else {
        LOGCATE("OnCameraFrameDataValible not support type:%d",type);
    }


}

void HwMediaMuxer::OnAudioData(uint8_t *audioData, jint length) {
    std::unique_lock<std::mutex> uniqueLock(HwMediaMuxer::getInstace() -> runningMutex,std::defer_lock);
    uniqueLock.lock();
    if (isDestroyed){
        uniqueLock.unlock();
        return;
    }
    uniqueLock.unlock();

    auto * data = new uint8_t [length];
    memcpy(data,audioData,length);
    if (getInstace()->mediaCodecA){
        getInstace()->mediaCodecA->putData(data,length);
    }
}

void HwMediaMuxer::OnDrawFrame(){
    if (videoRender) videoRender->DrawFrame();
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
    if(videoRender){
        videoRender->Destroy();
        delete videoRender;
        videoRender = 0;
    }
    mediaCodecA->destroy();
    delete mediaCodecA;
    mediaCodecV->destroy();
    delete mediaCodecV;
    mMuxerHelper->stop();
    mMuxerHelper->destroy();
    delete mMuxerHelper;
    delete instance;
    instance = 0;
}
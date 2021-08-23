//
// Created by Admin on 2021/5/31.
//



#include <utils/utils.h>
#include <utils/CustomGLUtils.h>
#include <render/VideoRender.h>
#include <libyuv.h>
#include <libyuv/scale.h>
#include <libyuv/convert.h>
#include <libyuv/rotate.h>
#include <render/VideoFboRender.h>
#include "../../play_header/encoder/RtmpLiveMuxer.h"


RtmpLiveMuxer* RtmpLiveMuxer::instance = nullptr;

/**************************************************************/
/* media file output */

int RtmpLiveMuxer::init(const char * fileName){
    strcpy(mTargetFilePath,fileName);
    LOGCATE("打印地址:%s",fileName);
    access(fileName,0);
    mAvEncoder = new EncoderAACAndx264;
    mAvEncoder->init();
    return 0;
}


void RtmpLiveMuxer::receivePixelData(int type,NativeOpenGLImage *pVoid){
    RtmpLiveMuxer::getInstance()->OnCameraFrameDataValible(type, pVoid);
}


void RtmpLiveMuxer::OnSurfaceCreate() {
    videoRender = new VideoFboRender;
    videoRender->readPixelCall = RtmpLiveMuxer::receivePixelData;
    videoRender->Init();
}

void RtmpLiveMuxer::OnSurfaceChanged(int width,int height) {
    videoRender->OnSurfaceChanged(width,height);
}

void RtmpLiveMuxer::OnCameraFrameDataValible(int type,NativeOpenGLImage * srcData) {
    // 4-glreadPixel 读出来的数据已经转换成720分辨率的420p的数据了,可以直接编码
    std::unique_lock<std::mutex> uniqueLock(RtmpLiveMuxer::getInstance() -> runningMutex,std::defer_lock);
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
//        LOGCATE("log has received data:%d",type);
        NativeOpenGLImageUtil::FreeNativeImage(srcData);
        delete srcData;
    } else if (type == 4) {
        // glreadypixel 后转换成720分辨率的420p的画面,可以直接使用
//        LOGCATE("log has received data:%d",type);
        NativeOpenGLImageUtil::FreeNativeImage(srcData);
        delete srcData;
    } else {
        LOGCATE("OnCameraFrameDataValible not support type:%d",type);
    }
}

void RtmpLiveMuxer::OnAudioData(uint8_t *audioData, jint length) {
    std::unique_lock<std::mutex> uniqueLock(RtmpLiveMuxer::getInstance() -> runningMutex,std::defer_lock);
    uniqueLock.lock();
    if (isDestroyed){
        uniqueLock.unlock();
        return;
    }
    uniqueLock.unlock();

    auto * data = new uint8_t [length];
    memcpy(data,audioData,length);

    // 处理数据
}

void RtmpLiveMuxer::OnDrawFrame(){
    if (videoRender) videoRender->DrawFrame();
}

void RtmpLiveMuxer::Destroy(){
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
    if (mAvEncoder){
        mAvEncoder->destroy();
        delete mAvEncoder;
    }
    delete instance;
    instance = 0;
}
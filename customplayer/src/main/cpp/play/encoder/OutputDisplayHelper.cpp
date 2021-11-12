//
// Created by Admin on 2021/11/9.
//
#include <encoder/OutputDisplayHelper.h>
#include <render/VideoFboRender.h>

OutputDisplayHelper* OutputDisplayHelper::instance = nullptr;

OutputDisplayHelper * OutputDisplayHelper::getInstance(){
    if (instance == nullptr) {
        instance = new OutputDisplayHelper;
    }
    return instance;
}

void OutputDisplayHelper::init(){
    soundTouchHelper = new SoundTouchHelper;
    soundTouchHelper->init();
    soundTouchHelper->setAudioCallback(reinterpret_cast<ReceiveAudioData>(OutputDisplayHelper::receiveSoundTouchData));
}

void OutputDisplayHelper::destroy(){
    std::lock_guard<std::mutex> lockGuard(runningMutex);
    if (isDestroyed) return;
    isDestroyed = true;
    if(videoRender){
        videoRender->Destroy();
        delete videoRender;
        videoRender = 0;
    }
    soundTouchHelper->destroy();
    delete soundTouchHelper;
}

void OutputDisplayHelper::setSpeed(float speed){
    this->speed = speed;
    soundTouchHelper->setSpeed(speed);
}

void OutputDisplayHelper::receivePixelData(int type,NativeOpenGLImage *pVoid){
    OutputDisplayHelper::getInstance()->OnCameraFrameDataValible(type, pVoid);
}

void OutputDisplayHelper::setOutputListener(OutputAvData outputAvData) {
    this->outputAvData = outputAvData;
}

void OutputDisplayHelper::OnSurfaceCreate() {
    videoRender = new VideoFboRender;
    videoRender->readPixelCall = OutputDisplayHelper::receivePixelData;
    videoRender->Init();
}
void OutputDisplayHelper::OnSurfaceChanged(int width,int height) {
    videoRender->OnSurfaceChanged(width,height);
}
void OutputDisplayHelper::OnCameraFrameDataValible(int type,NativeOpenGLImage* srcData) {
    // 4-glreadPixel 读出来的数据已经转换成720分辨率的420p的数据了,可以直接编码
    std::unique_lock<std::mutex> uniqueLock(OutputDisplayHelper::getInstance() -> runningMutex,std::defer_lock);
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
        if (outputAvData != nullptr) {
            outputAvData(2,srcData,0);
        } else {
            NativeOpenGLImageUtil::FreeNativeImage(srcData);
            delete srcData;
        }
    } else if (type == 4) {
        // glreadypixel 后转换成720分辨率的420p的画面,可以直接使用
        if (outputAvData != nullptr) {
            outputAvData(2,srcData,0);
        } else {
            NativeOpenGLImageUtil::FreeNativeImage(srcData);
            delete srcData;
        }
    } else {
        LOGCATE("OnCameraFrameDataValible not support type:%d",type);
    }
}
void OutputDisplayHelper::OnDrawFrame() {
    if (videoRender) videoRender->DrawFrame();
}
void OutputDisplayHelper::OnAudioData(uint8_t *audioData, int length) {
    std::unique_lock<std::mutex> uniqueLock(OutputDisplayHelper::getInstance() -> runningMutex,std::defer_lock);
    uniqueLock.lock();
    if (isDestroyed){
        uniqueLock.unlock();
        return;
    }
    uniqueLock.unlock();

    if (speed != 1.0) {
        soundTouchHelper->adjustAudioData(audioData,length);
    } else {
        if (getInstance()->outputAvData != nullptr) {
            getInstance() -> outputAvData(1,audioData,length);
        }
    }
}

void OutputDisplayHelper::receiveSoundTouchData(short * srcData,int size){
    std::unique_lock<std::mutex> uniqueLock(OutputDisplayHelper::getInstance() -> runningMutex,std::defer_lock);
    uniqueLock.lock();
    if (OutputDisplayHelper::getInstance()->isDestroyed){
        uniqueLock.unlock();
        return;
    }
    uniqueLock.unlock();

    if (getInstance()->outputAvData != nullptr) {
        getInstance() -> outputAvData(1,srcData,size);
    }
}


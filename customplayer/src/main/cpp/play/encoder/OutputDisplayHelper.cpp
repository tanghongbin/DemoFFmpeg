//
// Created by Admin on 2021/11/9.
//


#include <encoder/OutputDisplayHelper.h>

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

OutputDisplayHelper::~OutputDisplayHelper(){
    if (isDestroyed) return;
    if(videoRender){
        videoRender->Destroy();
        delete videoRender;
        videoRender = 0;
    }
    if (soundTouchHelper) {
        soundTouchHelper->destroy();
        delete soundTouchHelper;
    }
    isDestroyed = true;
    instance = 0;
}

void OutputDisplayHelper::setSpeed(float speed){
    this->speed = speed;
    soundTouchHelper->setSpeed(speed);
}

/***
 * 最终处理数据的一方进行拷贝
 * @param type
 * @param srcData
 */
void OutputDisplayHelper::receivePixelData(int type,NativeOpenGLImage *srcData){
    // 3-rgba放进队列
    if (getInstance()->videoCall == nullptr) {
        return;
    }
    getInstance()->videoCall(2,srcData);
}

void OutputDisplayHelper::setOutputAudioListener(ReceiveAudioData outputAvData) {
    this->audioCall = outputAvData;
}

void OutputDisplayHelper::setOutputVideoListener(ReceiveOqTypeData outputAvData) {
    this->videoCall = outputAvData;
}

void OutputDisplayHelper::OnSurfaceCreate() {
    videoRender = new VideoFboOESRender;
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
    if (getInstance()->audioCall == nullptr) return;
    if (speed != 1.0) {
        soundTouchHelper->adjustAudioData(audioData,length);
    } else {
        getInstance() -> audioCall(audioData,length);
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
    getInstance() -> audioCall(srcData,size);
}

void OutputDisplayHelper::UpdateOESMartix(float *pDouble) {
    if (videoRender) videoRender->UpdateOESMartix(pDouble);
}


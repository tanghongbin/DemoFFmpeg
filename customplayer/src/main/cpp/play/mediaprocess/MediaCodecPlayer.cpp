//
// Created by Admin on 2021/5/13.
//

#include <mediaprocess/MediaCodecPlayer.h>
#include <render/VideoRender.h>
#include <decoder/ImlFFmpegDecoder.h>
#include <decoder/hw/ImlHwDecoder.h>

MediaCodecPlayer::MediaCodecPlayer(){
    audioDecoder =  videoDecoder = 0;
    mNativeWindow = 0;
}

void MediaCodecPlayer::Init(){
//    audioDecoder = HwAudioDecoder::getInstance();
//    audioDecoder->setReadyCall(prepareReady);
//    audioDecoder->setMediaType(1);
    videoDecoder = new HwVideoDecoder;
    videoDecoder->setReadyCall(prepareReady);
    videoDecoder->setMediaType(2);
}

void MediaCodecPlayer::OnSurfaceCreated() {
    if (!videoDecoder) return;
    return;
    BaseDecoder* videoResult = videoDecoder;
    auto *render = new VideoRender;
    render->Init();
    videoResult->setVideoRender(render);
}

void MediaCodecPlayer::OnSurfaceChanged(int oretation,int width, int height)  {
    if (!videoDecoder) return;
    return;
    if (videoDecoder) {
        auto* videoResult = dynamic_cast<BaseHwDecoder *>(videoDecoder);
        videoResult->OnSurfaceChanged(oretation,width,height);
    }
    std::unique_lock<std::mutex> uniqueLock(videoDecoder->mCreateSurfaceMutex);
    videoDecoder->mSurfaceCondition.notify_one();
    uniqueLock.unlock();
}

void MediaCodecPlayer::OnDrawFrame() {
    if (videoDecoder && videoDecoder->videoRender){
        videoDecoder->videoRender->DrawFrame();
    }
}

void MediaCodecPlayer::Destroy() {
    if (audioDecoder) {
        audioDecoder->Destroy();
        HwAudioDecoder::destroyInstance();
        audioDecoder = 0;
    }
    if (videoDecoder) {
        videoDecoder->Destroy();
        delete videoDecoder;
        videoDecoder = 0;
    }
}

void MediaCodecPlayer::Prepare() {
    if (audioDecoder) audioDecoder->Init(mUrl);
    if (videoDecoder) videoDecoder->Init(mUrl);
}
void MediaCodecPlayer::Start() {
    if (audioDecoder) audioDecoder->Start();
    if (videoDecoder)  videoDecoder->Start();
}
void MediaCodecPlayer::Stop() {
    if (audioDecoder) audioDecoder->Stop();
    if (videoDecoder)  videoDecoder->Stop();
}
void MediaCodecPlayer::Reset() {
    if (audioDecoder) audioDecoder->Reset();
    if (videoDecoder)  videoDecoder->Reset();
}
void MediaCodecPlayer::SeekTo(int second) {
    if (audioDecoder) audioDecoder->ManualSeekPosition(second);
    if (videoDecoder)  videoDecoder->ManualSeekPosition(second);
}

void MediaCodecPlayer::Replay() {
    if (audioDecoder) audioDecoder->Replay();
    if (videoDecoder) videoDecoder->Replay();
}

void MediaCodecPlayer::setNativeWindow(jobject surface){
    if (mNativeWindow) {
        ANativeWindow_release(mNativeWindow);
        mNativeWindow = 0;
    }
    bool isAttach;
    mNativeWindow = ANativeWindow_fromSurface(JavaVmManager::GetEnv(&isAttach),surface);
    videoDecoder->nativeWindow = mNativeWindow;
//    if (isAttach) {
//        JavaVmManager::detachCurrentThread();
//    }
}

void MediaCodecPlayer::deleteNativeWindow(){
    if (mNativeWindow) {
        ANativeWindow_release(mNativeWindow);
        mNativeWindow = 0;
    }
}


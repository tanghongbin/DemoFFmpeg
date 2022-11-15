//
// Created by Admin on 2021/5/13.
//

#include <mediaprocess/MediaCodecPlayer.h>
#include <render/VideoRender.h>
#include <decoder/ImlFFmpegDecoder.h>
#include <decoder/hw/ImlHwDecoder.h>
#include <render/VideoFboRender.h>
#include <render/VideoSpecialEffortsRender.h>

MediaCodecPlayer* MediaCodecPlayer::instance = 0;

MediaCodecPlayer::MediaCodecPlayer(){
    audioDecoder =  videoDecoder = specialEffortDecoder = 0;
}

 
MediaCodecPlayer* MediaCodecPlayer::getInstance(){
    if (instance == nullptr) {
        instance = new MediaCodecPlayer;
    }
    return instance;
}

void MediaCodecPlayer::test(void (MediaCodecPlayer::*nonstatic)()){
    (this->*nonstatic)();
}

void MediaCodecPlayer::Init(){
    audioDecoder = HwAudioDecoder::getInstance();
    audioDecoder->setReadyCall(prepareReady);
    audioDecoder->setMediaType(1);
    videoDecoder = new HwVideoDecoder;
    videoDecoder->setReadyCall(prepareReady);
    videoDecoder->setMediaType(2);
}

void MediaCodecPlayer::OnSurfaceCreated() {
    if (!videoDecoder) return;
    BaseDecoder* videoResult = videoDecoder;
    auto *render = new VideoRender;
    render->Init();
    videoResult->setVideoRender(render);
}

void MediaCodecPlayer::OnSurfaceChanged(int oretation,int width, int height)  {
    if (!videoDecoder) return;
    auto* videoResult = dynamic_cast<BaseHwDecoder *>(videoDecoder);
    videoResult->OnSurfaceChanged(oretation,width,height);
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
    if (specialEffortDecoder) {
        specialEffortDecoder->Destroy();
        delete specialEffortDecoder;
        specialEffortDecoder = 0;
    }
    instance = nullptr;
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

void MediaCodecPlayer::ApplyEfforts(const char * url){
    if (specialEffortDecoder) {
        specialEffortDecoder->Stop();
        specialEffortDecoder->Destroy();
        delete specialEffortDecoder;
        specialEffortDecoder = 0;
    }
    specialEffortDecoder = new HwVideoDecoder;
    specialEffortDecoder->setMediaType(2);
    specialEffortDecoder->setIfNeedRender(false);
    specialEffortDecoder->setIfNeedPauseWhenFinished(false);
    specialEffortDecoder->setOutputDataListener(MediaCodecPlayer::receiveSpecialEffortsData);
    specialEffortDecoder->setOnCompleteCall(this);
    specialEffortDecoder->Init(url);
    specialEffortDecoder->Start();
    auto *videoRender = dynamic_cast<VideoSpecialEffortsRender *>(getInstance()->videoDecoder->videoRender);
    videoRender->setReadResultListener(this);
}

/***
 * 播放完成的回调
 */
void MediaCodecPlayer::call() {
    auto *videoRender = dynamic_cast<VideoSpecialEffortsRender *>(videoDecoder->videoRender);
    videoRender->destroySpecialEffortsImage();
    LOGCATE("已经销毁特效");
}

void MediaCodecPlayer::readPixelResult(NativeOpenGLImage* data){

}
 
void MediaCodecPlayer::receiveSpecialEffortsData(int type,AMediaCodecBufferInfo* info,uint8_t* data){
    if (type != 2) return;
    int vWidth = getInstance()->specialEffortDecoder->mVideoWidth;
    int vHeight =  getInstance()->specialEffortDecoder->mVideoHeight;
    uint8_t * i420dst = new uint8_t [vWidth * vHeight *3/2];
    yuvNv12ToI420(data,i420dst,vWidth,vHeight);
//    LOGCATE("nv12toi420 耗费时间:%lld",(GetSysCurrentTime() - start));
    NativeOpenGLImage openGlImage;
    openGlImage.width = vWidth;
    openGlImage.height = vHeight;
    openGlImage.ppPlane[0] = i420dst;
    openGlImage.ppPlane[1] = i420dst + vWidth * vHeight;
    openGlImage.ppPlane[2] = i420dst + vWidth * vHeight * 5 / 4;
    openGlImage.pLineSize[0]= vWidth;
    openGlImage.pLineSize[1]= vWidth / 2;
    openGlImage.pLineSize[2]= vWidth / 2;
    openGlImage.format = IMAGE_FORMAT_I420;
    auto *videoRender = dynamic_cast<VideoSpecialEffortsRender *>(getInstance()->videoDecoder->videoRender);
    videoRender->copySpecialEffortsImage(&openGlImage);
    delete [] i420dst;
}

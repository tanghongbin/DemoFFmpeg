//
// Created by Admin on 2021/5/13.
//

#include <mediaprocess/FFmpegMediaPlayer.h>
#include <decoder/ImlFFmpegDecoder.h>
#include <render/FFmpegVideoToRgbConverter.h>

void FFmpegMediaPlayer::Init(){
    audioDecoder = AudioFFmpegDecoder::getInstance();
    audioDecoder->call = prepareReady;
    audioDecoder->setMediaType(1);
    videoDecoder = new VideoFFmpegDecoder;
    videoDecoder->call = prepareReady;
    videoDecoder->setMediaType(2);
}

void FFmpegMediaPlayer::OnSurfaceCreated() {
    BaseDecoder* videoResult = videoDecoder;
    auto *render = new VideoRender;
    render->Init();
    videoResult->setVideoRender(render);
    LOGCATE("OnSurfaceCreated create render success:%p",render);
}

void FFmpegMediaPlayer::OnSurfaceChanged(int oreration,int width, int height)  {
    std::unique_lock<std::mutex> uniqueLock(videoDecoder->createSurfaceMutex);
    videoDecoder->createSurfaceCondition.notify_one();
    uniqueLock.unlock();
    if (videoDecoder) {
        VideoFFmpegDecoder* videoResult = dynamic_cast<VideoFFmpegDecoder *>(videoDecoder);
        videoResult->OnSurfaceChanged(oreration,width,height);
    }
}

void FFmpegMediaPlayer::OnDrawFrame() {
    if (videoDecoder){
        VideoFFmpegDecoder* decoder = dynamic_cast<VideoFFmpegDecoder *>(videoDecoder);
        decoder->drawVideoFrame();
    }
}

void FFmpegMediaPlayer::Destroy() {
    audioDecoder->Destroy();
    AudioFFmpegDecoder::destroyInstance();
    audioDecoder = 0;
    videoDecoder->Destroy();
    delete videoDecoder;
    videoDecoder = 0;
}

void FFmpegMediaPlayer::Prepare() {
    LOGCATE("start FFmpegMediaPlayer prepare");
    prepareCount = 0;
    audioDecoder->Init(mUrl);
    videoDecoder->Init(mUrl);
}

void FFmpegMediaPlayer::Start() {
    audioDecoder->Start();
    videoDecoder->Start();
}

void FFmpegMediaPlayer::Stop() {
    audioDecoder->Stop();
    videoDecoder->Stop();
}

void FFmpegMediaPlayer::Reset(){
    audioDecoder->Reset();
    videoDecoder->Reset();
}

void FFmpegMediaPlayer::SeekTo(int second) {
    audioDecoder->ManualSeekPosition(second);
    videoDecoder->ManualSeekPosition(second);
}

void FFmpegMediaPlayer::Replay(){
    audioDecoder->Replay();
    videoDecoder->Replay();
}

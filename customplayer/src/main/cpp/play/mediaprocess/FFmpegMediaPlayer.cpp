//
// Created by Admin on 2021/5/13.
//

#include <mediaprocess/FFmpegMediaPlayer.h>
#include <decoder/ImlDecoder.h>
#include <render/VideoDataConverter.h>

void FFmpegMediaPlayer::Init(){
    audioDecoder = new AudioDecoder;
    audioDecoder->call = prepareReady;
    audioDecoder->setMediaType(1);
    videoDecoder = new VideoDecoder;
    videoDecoder->call = prepareReady;
    videoDecoder->setMediaType(2);
}

void FFmpegMediaPlayer::OnSurfaceCreated() {
    VideoDecoder* videoResult = dynamic_cast<VideoDecoder *>(videoDecoder);
    VideoRender *render = new VideoRender;
    render->Init();
    std::unique_lock<std::mutex> uniqueLock(videoDecoder->createSurfaceMutex);
    videoResult->videoRender = render;
    videoDecoder->createSurfaceCondition.notify_one();
    uniqueLock.unlock();
    LOGCATE("OnSurfaceCreated create render success:%p videoResult:%p",render,videoResult->videoRender);
}

void FFmpegMediaPlayer::OnSurfaceChanged(int width, int height)  {

}

void FFmpegMediaPlayer::OnDrawFrame() {
    if (videoDecoder){
        VideoDecoder* decoder = dynamic_cast<VideoDecoder *>(videoDecoder);
        decoder->drawVideoFrame();
    }
}

void FFmpegMediaPlayer::Destroy() {
    audioDecoder->Destroy();
    delete audioDecoder;
    audioDecoder = 0;
    videoDecoder->Destroy();
    delete videoDecoder;
    videoDecoder = 0;
}

void FFmpegMediaPlayer::Prepare() {
    LOGCATE("start FFmpegMediaPlayer prepare");
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

void FFmpegMediaPlayer::SeekTo(int second) {
    audioDecoder->ManualSeekPosition(second);
    videoDecoder->ManualSeekPosition(second);
}

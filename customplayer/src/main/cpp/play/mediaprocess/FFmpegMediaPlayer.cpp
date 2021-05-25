//
// Created by Admin on 2021/5/13.
//

#include <mediaprocess/FFmpegMediaPlayer.h>
#include <decoder/AudioDecoder.h>

void FFmpegMediaPlayer::Init(){

}

void FFmpegMediaPlayer::OnSurfaceCreated() {

}

void FFmpegMediaPlayer::OnSurfaceChanged(int width, int height)  {

}

void FFmpegMediaPlayer::OnDrawFrame() {

}

void FFmpegMediaPlayer::Destroy() {

}

void FFmpegMediaPlayer::Prepare() {
    LOGCATE("start FFmpegMediaPlayer prepare");
    audioDecoder = new AudioDecoder;
    audioDecoder->call = prepareReady;
    audioDecoder->setMediaType(1);
    audioDecoder->Init(mUrl);
}

void FFmpegMediaPlayer::Start() {

}

void FFmpegMediaPlayer::Stop() {

}

void FFmpegMediaPlayer::SeekTo(long second) {

}

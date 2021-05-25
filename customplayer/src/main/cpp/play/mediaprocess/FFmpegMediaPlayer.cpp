//
// Created by Admin on 2021/5/13.
//

#include <mediaprocess/FFmpegMediaPlayer.h>
#include <decoder/ImlDecoder.h>

void FFmpegMediaPlayer::Init(){

}

void FFmpegMediaPlayer::OnSurfaceCreated() {

}

void FFmpegMediaPlayer::OnSurfaceChanged(int width, int height)  {

}

void FFmpegMediaPlayer::OnDrawFrame() {

}

void FFmpegMediaPlayer::Destroy() {
    audioDecoder->Destroy();
    delete audioDecoder;
}

void FFmpegMediaPlayer::Prepare() {
    LOGCATE("start FFmpegMediaPlayer prepare");
    audioDecoder = new AudioDecoder;
    audioDecoder->call = prepareReady;
    audioDecoder->setMediaType(1);
    audioDecoder->Init(mUrl);
}

void FFmpegMediaPlayer::Start() {
    audioDecoder->Start();
}

void FFmpegMediaPlayer::Stop() {
    audioDecoder->Stop();
}

void FFmpegMediaPlayer::SeekTo(int second) {
    audioDecoder->ManualSeekPosition(second);
}

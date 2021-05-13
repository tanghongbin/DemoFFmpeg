//
// Created by Admin on 2021/5/13.
//

#include <mediaprocess/MediaCodecPlayer.h>
#include <render/VideoRender.h>

void MediaCodecPlayer::Init(){
    videoRender = new VideoRender;
    videoRender->Init();
}

void MediaCodecPlayer::OnSurfaceCreated() {

}

void MediaCodecPlayer::OnSurfaceChanged(int width, int height)  {

}

void MediaCodecPlayer::OnDrawFrame() {
    videoRender->DrawFrame();
}

void MediaCodecPlayer::Destroy() {
    videoRender->Destroy();
    delete videoRender;
}


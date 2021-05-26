//
// Created by Admin on 2021/5/25.
//

#include <render/VideoDataConverter.h>
#include <utils/utils.h>
#include "utils/OpenGLImageDef.h"


void VideoDataConverter::Init(AVCodecContext* codeCtx){
    targetWidth = 1280;
    targetHeight = 680;
    swsCtx = sws_getContext(codeCtx->width,codeCtx->height,codeCtx->pix_fmt,targetWidth,targetHeight,
            AV_PIX_FMT_RGBA,SWS_FAST_BILINEAR,NULL,NULL,NULL);
    targetFrame = av_frame_alloc();
    int bufferSize = av_image_get_buffer_size(AV_PIX_FMT_RGBA,targetWidth,targetHeight,1);
    targetData = static_cast<uint8_t *>(av_malloc(bufferSize));
    int ret = av_image_fill_arrays(targetFrame->data,targetFrame->linesize,targetData,
            AV_PIX_FMT_RGBA,targetWidth,targetHeight,1);
    if (ret < 0){
        LOGCATE("create video data coverter error:%s",av_err2str(ret));
    }
}

void VideoDataConverter::Destroy(){
    LOGCATE("targetFrame:%p targetData:%p swsCtx:%p videoRender:%p",targetFrame,targetData,swsCtx,videoRender);
    isDestroyed = true;
    if (targetFrame) av_frame_free(&targetFrame);
    av_free(targetData);
    sws_freeContext(swsCtx);
    if (videoRender){
        videoRender->Destroy();
        delete videoRender;
        videoRender = nullptr;
    }
}

void VideoDataConverter::covertData(AVFrame* data){
    int ret = sws_scale(swsCtx,data->data,data->linesize,targetFrame->width,targetFrame->height,
            targetFrame->data,targetFrame->linesize);
//    LOGCATE("log sws_scale result:%s",av_err2str(ret));
    if (videoRender) videoRender->copyImage(data);

}

void VideoDataConverter::drawVideoFrame(){
    if (isDestroyed) return;
    if (videoRender) videoRender->DrawFrame();
}

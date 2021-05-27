//
// Created by Admin on 2021/5/25.
//

#include <render/VideoDataConverter.h>
#include <utils/utils.h>
#include "utils/OpenGLImageDef.h"


void VideoDataConverter::Init(AVCodecContext* codeCtx){
    targetWidth = codeCtx->width;
    targetHeight = codeCtx->height;
    videoWidth = codeCtx->width;
    videoHeight = codeCtx->height;
    swsCtx = sws_getContext(codeCtx->width,codeCtx->height,codeCtx->pix_fmt,targetWidth,targetHeight,
            AV_PIX_FMT_RGBA,SWS_FAST_BILINEAR,NULL,NULL,NULL);
    targetFrame = av_frame_alloc();
    int bufferSize = av_image_get_buffer_size(AV_PIX_FMT_RGBA,targetWidth,targetHeight,1);
    targetData = (uint8_t *) av_malloc(bufferSize * sizeof(uint8_t));
    int ret = av_image_fill_arrays(targetFrame->data,targetFrame->linesize,targetData,
            AV_PIX_FMT_RGBA,targetWidth,targetHeight,1);
    LOGCATE("video info width:%d height:%d targetWidth:%d targetHeight:%d videoFmt:%d targetLinesize:%d",
            codeCtx->width,codeCtx->height,targetWidth,targetHeight,codeCtx->pix_fmt,targetFrame->linesize[0]);
    if (ret < 0){
        LOGCATE("create video data coverter error:%s",av_err2str(ret));
    }
}

void VideoDataConverter::Destroy(){
//    LOGCATE("targetFrame:%p targetData:%p swsCtx:%p videoRender:%p",targetFrame,targetData,swsCtx,videoRender);
    isDestroyed = true;
    if (targetFrame) av_frame_free(&targetFrame);
    av_free(targetData);
    sws_freeContext(swsCtx);

}

void VideoDataConverter::covertData(AVFrame* srcFrame){
    if (isDestroyed) return;
//    int ret = sws_scale(swsCtx,data->data,data->linesize,targetFrame->width,targetFrame->height,
//            targetFrame->data,targetFrame->linesize);
//    int ret = sws_scale(swsCtx, srcFrame->data, srcFrame->linesize, 0,
//                        videoHeight, targetFrame->data, targetFrame->linesize);
    LOGCATE("log src lineSize:%d videoHeight:%d targetLineSize:%d",srcFrame->linesize[0],
            videoHeight,targetFrame->linesize[0]);
//    if (ret < 0){
//        LOGCATE("log scale data height:%d scale result:%s",srcFrame->height,av_err2str(ret));
//    }
    NativeOpenGLImage targetIamge;
    targetIamge.width = targetWidth;
    targetIamge.height = targetHeight;
    targetIamge.format = IMAGE_FORMAT_RGBA;
    targetIamge.ppPlane[0] = targetFrame->data[0];
//    LOGCATE("log sws_scale result:%s",av_err2str(ret));


}

void VideoDataConverter::drawVideoFrame(){

}

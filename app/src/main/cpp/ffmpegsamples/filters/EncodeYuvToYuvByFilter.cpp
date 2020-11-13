//
// Created by Admin on 2020/11/12.
//

#include <cstdio>
#include <CustomGLUtils.h>

#include <encode/EncodeYuvToJpg.h>
#include "EncodeYuvToYuvByFilter.h"
#include "../../utils/utils.h"
#include "WaterFilterHelper.h"

extern "C"{
#include <libavutil/frame.h>
#include <libavutil/imgutils.h>
#include <libavfilter/buffersrc.h>
#include <libavfilter/buffersink.h>
}


const char *EncodeYuvToYuvByFilter::yuvToyuvByFilter(const char *inputName) {
    LOGCATE("i'm success to come in:%s", inputName);
    int ret;
    int width = 840, height = 1074;
    const char *ERROR_RESULT = "";
    FILE *inFile = fopen(inputName, "rb");
    if (!inFile) {
        LOGCATE("can't open inputfile");
        return ERROR_RESULT;
    }
    const char *outFileName = getRandomStr("filteryuvtoyuv-", ".yuv","yuvfiles/");

    LOGCATE("log outFileName:%s targetOut:%s",outFileName,outFileName);
    FILE *outFile = fopen(outFileName, "wb");
    if (!outFile) {
        LOGCATE("can't open outFile");
        return ERROR_RESULT;
    }
    LOGCATE("log again outFileName:%s",outFileName);
    AVFrame *inFrame = av_frame_alloc();
    AVFrame *outFrame = av_frame_alloc();
    inFrame->width = width;
    inFrame->height = height;
    inFrame->format = AV_PIX_FMT_YUV420P;

    outFrame->width = width;
    outFrame->height = height;
    outFrame->format = AV_PIX_FMT_YUV420P;
    uint8_t *in_buffer = fillArrayToFrame(AV_PIX_FMT_YUV420P, inFrame);
    uint8_t *out_buffer = fillArrayToFrame(AV_PIX_FMT_YUV420P, outFrame);

    int size = av_image_get_buffer_size(AV_PIX_FMT_YUV420P, inFrame->width, inFrame->height, 1);
    if (fread(in_buffer, 1, size, inFile)!= size) {
        LOGCATE("read in buffer error");
        return ERROR_RESULT;
    }
    LOGCATE("log yuv in_file size:%d",size);
    //input Y,U,V
    inFrame->data[0] = in_buffer;
    inFrame->data[1] = in_buffer + width * height;
    inFrame->data[2] = in_buffer + width * height * 5 / 4;

    WaterFilterHelper *waterFilterHelper = new WaterFilterHelper;
    ret = waterFilterHelper->initWaterFilter(width, height);
    if (ret < 0){
        LOGCATE("init waterfilter error:%s",av_err2str(ret));
        return ERROR_RESULT;
    }
    long long startTime = GetSysCurrentTime();
    if ((ret = av_buffersrc_add_frame(waterFilterHelper->buffersrc_ctx, inFrame)) < 0) {
        LOGCATE("av_buffersrc_add_frame failed:%s",av_err2str(ret));
        return ERROR_RESULT;
    }


    ret = av_buffersink_get_frame(waterFilterHelper->buffersink_ctx, outFrame);
    if (ret < 0){
        LOGCATE("av_buffersink_get_frame error:%s",av_err2str(ret));
        return ERROR_RESULT;
    }

    LOGCATE("查看滤镜耗时：%lld",GetSysCurrentTime() - startTime);

    if(outFrame->format==AV_PIX_FMT_YUV420P){
        for(int i=0;i<outFrame->height;i++){
            fwrite(outFrame->data[0]+outFrame->linesize[0]*i,1,outFrame->width,outFile);
        }
        for(int i=0;i<outFrame->height/2;i++){
            fwrite(outFrame->data[1]+outFrame->linesize[1]*i,1,outFrame->width/2,outFile);
        }
        for(int i=0;i<outFrame->height/2;i++){
            fwrite(outFrame->data[2]+outFrame->linesize[2]*i,1,outFrame->width/2,outFile);
        }
    }
    av_frame_unref(inFrame);
    av_frame_unref(outFrame);
    fclose(inFile);
    fclose(outFile);


    waterFilterHelper->unInit();
    delete waterFilterHelper;

    LOGCATE("打印最终输出3yuv:   %s   targetout:%s",outFileName,outFileName);

    EncodeYuvToJpg* yuvToJpg = new EncodeYuvToJpg;
    const char * finalResult = yuvToJpg->encode(outFileName);
    delete yuvToJpg;
//    ret = remove(outFileName);
//    if (ret < 0){
//        LOGCATE("删除文件失败");
//    } else {
//        LOGCATE("文件删除成功");
//    }
    LOGCATE("打印最终输出jpg:   %s",finalResult);
    delete outFileName;

    return finalResult;
}
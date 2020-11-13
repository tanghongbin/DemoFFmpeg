//
// Created by Admin on 2020/11/13.
//

#include <cstdint>
#include <encode/EncodeYuvToJpg.h>
#include <utils.h>
#include <CustomGLUtils.h>

#include "SwsConvertYuvToRgb.h"

extern "C"{
#include <libavutil/imgutils.h>
}


void SwsConvertYuvToRgb::convert() {


    const uint8_t *ERROR_RESULT = nullptr;
    const char *inputFileName = COMMON_YUV_IMAGE_PATH;
    const char *finalResult;
    FILE *in_file;
    FILE *out_file;

    uint8_t *pic_buf;
    int ret;
    uint8_t endcode[] = {0, 0, 1, 0xb7};

    const char *outputFileName = getRandomStr("",".rgb","filterImg/");

    // 先读取文件RGBA数据到frame中
    in_file = fopen(inputFileName, "rb");
    if (!in_file) {
        LOGCATE("Could not open input file %s\n", inputFileName);
        return ;
    }
    out_file = fopen(outputFileName, "wb");
    if (!out_file) {
        LOGCATE("Could not open out file %s\n", inputFileName);
        return ;
    }

    int originalWidth = 840,originalHeight = 1074,targetWidth = 1080,targetHeight = 1920;

    SwsHelper *swsHelper = new SwsHelper;
    swsHelper->init(originalWidth, originalHeight, AV_PIX_FMT_RGBA,
                    targetWidth, targetHeight, AV_PIX_FMT_RGBA, SWS_FAST_BILINEAR, NULL, NULL, NULL);

    int size = av_image_get_buffer_size(AV_PIX_FMT_RGBA,originalWidth,originalHeight,1);
    int linesize = av_image_get_linesize(AV_PIX_FMT_RGBA,originalWidth,1);
    uint8_t * src = static_cast<uint8_t *>(av_malloc(size));
    if (fread(src,1,size,in_file) <= 0){
        LOGCATE("read error");
        return;
    } else if (feof(in_file)){
        LOGCATE("read finished");
    }

    uint8_t * frame_buffer = static_cast<uint8_t *>(av_malloc(size));
    AVFrame* rgbFrame = av_frame_alloc();
    rgbFrame->format = AV_PIX_FMT_RGBA;
    rgbFrame->width = targetWidth;
    rgbFrame->height = targetHeight;
    av_image_fill_arrays(rgbFrame->data,rgbFrame->linesize,frame_buffer,AV_PIX_FMT_RGBA,targetWidth,targetHeight,1);

//    swsHelper->scale(src,linesize,0,originalHeight,frame_buffer,rgbFrame->linesize);
//
//    fwrite(scaleFrame->data[0], 1, size, out_file);
    fwrite(endcode, 1, sizeof(endcode), out_file);

    fclose(in_file);
    fclose(out_file);


    swsHelper->unInit();
    delete swsHelper;
    swsHelper = nullptr;

    LOGCATE("enough");
}
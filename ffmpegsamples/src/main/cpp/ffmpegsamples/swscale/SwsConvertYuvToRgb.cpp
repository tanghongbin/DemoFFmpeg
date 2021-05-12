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
    FILE *dst_file;

    uint8_t *pic_buf;
    int ret;
    uint8_t endcode[] = {0, 0, 1, 0xb7};

    const char *outputFileName = getRandomStr("",".rgb","filterImg/");

    LOGCATE("打印输出名字:%s",outputFileName);
    // 先读取文件RGBA数据到frame中
    in_file = fopen(inputFileName, "rb");
    if (!in_file) {
        LOGCATE("Could not open input file %s\n", inputFileName);
        return ;
    }
    dst_file = fopen(outputFileName, "wb");
    if (!dst_file) {
        LOGCATE("Could not open out file %s\n", inputFileName);
        return ;
    }

    AVPixelFormat srcPix = AV_PIX_FMT_YUV420P;
    AVPixelFormat dstPix = AV_PIX_FMT_RGB24;

    int src_w = 840,src_h = 1074,dst_w = 1080,dst_h = 1920;

    SwsHelper *swsHelper = new SwsHelper;
    swsHelper->init(src_w, src_h, srcPix,
                    dst_w, dst_h, dstPix, SWS_FAST_BILINEAR, NULL, NULL, NULL);

    //Colorspace
//    ret=sws_setColorspaceDetails(swsHelper->mSwsCtx,sws_getCoefficients(SWS_CS_ITU601),0,
//                                 sws_getCoefficients(SWS_CS_ITU709),0,
//                                 0, 1 << 16, 1 << 16);
//    if (ret==-1) {
//        LOGCATE( "Colorspace not support.\n");
//        return ;
//    }

    int size = av_image_get_buffer_size(srcPix,src_w,src_h,1);
//    int linesize = av_image_get_linesize(AV_PIX_FMT_RGBA,originalWidth,1);

    uint8_t * temp_buffer = static_cast<uint8_t *>(av_malloc(size));

    uint8_t * src_data[4];

    int src_lineSize[4];

    uint8_t * dst_data[4];

    int dst_lineSize[4];

    av_image_alloc(src_data,src_lineSize,src_h,src_h,srcPix,1);

    av_image_alloc(dst_data,dst_lineSize,dst_w,dst_h,dstPix,1);

    if (fread(temp_buffer,1,size,in_file) <= 0){
        LOGCATE("read error");
        return;
    } else if (feof(in_file)){
        LOGCATE("read finished");
    }

    // 向转换的输入buffer写入数据
    switch(srcPix){
        case AV_PIX_FMT_GRAY8:{
            memcpy(src_data[0],temp_buffer,src_w*src_h);
            break;
        }
        case AV_PIX_FMT_YUV420P:{
            memcpy(src_data[0],temp_buffer,src_w*src_h);                    //Y
            memcpy(src_data[1],temp_buffer+src_w*src_h,src_w*src_h/4);      //U
            memcpy(src_data[2],temp_buffer+src_w*src_h*5/4,src_w*src_h/4);  //V
            break;
        }
        case AV_PIX_FMT_YUV422P:{
            memcpy(src_data[0],temp_buffer,src_w*src_h);                    //Y
            memcpy(src_data[1],temp_buffer+src_w*src_h,src_w*src_h/2);      //U
            memcpy(src_data[2],temp_buffer+src_w*src_h*3/2,src_w*src_h/2);  //V
            break;
        }
        case AV_PIX_FMT_YUV444P:{
            memcpy(src_data[0],temp_buffer,src_w*src_h);                    //Y
            memcpy(src_data[1],temp_buffer+src_w*src_h,src_w*src_h);        //U
            memcpy(src_data[2],temp_buffer+src_w*src_h*2,src_w*src_h);      //V
            break;
        }
        case AV_PIX_FMT_YUYV422:{
            memcpy(src_data[0],temp_buffer,src_w*src_h*2);                  //Packed
            break;
        }
        case AV_PIX_FMT_RGB24:{
            memcpy(src_data[0],temp_buffer,src_w*src_h*3);                  //Packed
            break;
        }
        default:{
            printf("Not Support Input Pixel Format.\n");
            break;
        }
    }

//    sws_scale(img_convert_ctx, src_data, src_linesize, 0, src_h, dst_data, dst_linesize);
//    printf("Finish process frame %5d\n",frame_idx);
//    frame_idx++;

    long long startTime = GetSysCurrentTime();

    swsHelper->scale(src_data, src_lineSize, 0, src_h, dst_data, dst_lineSize);

    LOGCATE("total cost:%lld",GetSysCurrentTime() - startTime);
    switch(dstPix){
        case AV_PIX_FMT_GRAY8:{
            fwrite(dst_data[0],1,dst_w*dst_h,dst_file);
            break;
        }
        case AV_PIX_FMT_YUV420P:{
            fwrite(dst_data[0],1,dst_w*dst_h,dst_file);                 //Y
            fwrite(dst_data[1],1,dst_w*dst_h/4,dst_file);               //U
            fwrite(dst_data[2],1,dst_w*dst_h/4,dst_file);               //V
            break;
        }
        case AV_PIX_FMT_YUV422P:{
            fwrite(dst_data[0],1,dst_w*dst_h,dst_file);					//Y
            fwrite(dst_data[1],1,dst_w*dst_h/2,dst_file);				//U
            fwrite(dst_data[2],1,dst_w*dst_h/2,dst_file);				//V
            break;
        }
        case AV_PIX_FMT_YUV444P:{
            fwrite(dst_data[0],1,dst_w*dst_h,dst_file);                 //Y
            fwrite(dst_data[1],1,dst_w*dst_h,dst_file);                 //U
            fwrite(dst_data[2],1,dst_w*dst_h,dst_file);                 //V
            break;
        }
        case AV_PIX_FMT_YUYV422:{
            fwrite(dst_data[0],1,dst_w*dst_h*2,dst_file);               //Packed
            break;
        }
        case AV_PIX_FMT_RGB24:{
            fwrite(dst_data[0],1,dst_w*dst_h*3,dst_file);               //Packed
            break;
        }
        default:{
            printf("Not Support Output Pixel Format.\n");
            break;
        }
    }


    fclose(in_file);
    fclose(dst_file);


    swsHelper->unInit();
    delete swsHelper;
    swsHelper = nullptr;

    LOGCATE("convert success");

}
//
// Created by Admin on 2020/11/4.
//

#include <utils.h>
#include "SwsConvertYuvToPng.h"
#include "EncodeYuvToJpg.h"

extern "C" {
#include <libavutil/imgutils.h>
#include <libavformat/avformat.h>
}

/**
 * 单纯的对png进行缩放
 * @param sss
 * @return
 */
void SwsConvertYuvToPng::encode(const char *sss) {
    const uint8_t *ERROR_RESULT = nullptr;
    const char *inputFileName = OUTPUT_PNG_IMAGE_PATH;
    const char *finalResult;
    FILE *in_file;
    FILE *out_file;

    uint8_t *pic_buf;
    int ret;
    uint8_t endcode[] = {0, 0, 1, 0xb7};

    const char *outputFileName = OUTPUT_SWS_PNG_IMAGE_PATH;

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


void
SwsConvertYuvToPng::encodeInternal(AVCodecContext *pContext, AVFrame *pFrame, AVPacket *pPacket,
                                   FILE *pFile,
                                   SwsHelper *pHelper) {

    int ret;

    /* send the frame to the encoder */
//    if (frame)
//        LOGCATE("Send frame %3"PRId64"\n", frame->pts);

    ret = avcodec_send_frame(pContext, pFrame);
    if (ret < 0) {
        LOGCATE("Error sending a frame for encoding\n");
        return;
    }

    while (ret >= 0) {
        ret = avcodec_receive_packet(pContext, pPacket);
        if (ret == AVERROR(EAGAIN) || ret == AVERROR_EOF) {
            LOGCATE("Error during encoding,encode has end");
            return;
        } else if (ret < 0) {
            LOGCATE("Error during encoding\n");
            return;
        }

        LOGCATE("Write packet size:%d pktData:%p", pPacket->size, pPacket->data);
        fwrite(pPacket->data, 1, pPacket->size, pFile);
        av_packet_unref(pPacket);
    }
}
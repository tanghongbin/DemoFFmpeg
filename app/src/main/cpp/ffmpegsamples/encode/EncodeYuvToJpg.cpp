//
// Created by Admin on 2020/10/16.
//


#include <utils.h>
#include <CustomGLUtils.h>

#include "EncodeYuvToJpg.h"
#include "SwsHelper.h"

extern "C" {
#include <libavutil/imgutils.h>
#include <libavcodec/avcodec.h>
#include <libavutil/opt.h>
#include <libavformat/avformat.h>
}

const char *EncodeYuvToJpg::encode(const char *inputFileName) {
    const char *ERROR_RESULT = "";
    const char *finalResult;
    const AVCodec *codec;
    AVCodecContext *codeCtx = NULL;
    FILE *in_file;
    FILE *out_file;

    AVFrame *frame;
    AVPacket *pkt;
    uint8_t *pic_buf;
    int ret;
    uint8_t endcode[] = {0, 0, 1, 0xb7};

    const char *outputFileName = getRandomStr("encodejpg-",".jpg","filterImg/");
    // 先读取文件yuv数据到frame中
    in_file = fopen(inputFileName, "rb");
    if (!in_file) {
        LOGCATE("Could not open input file %s\n", inputFileName);
        return ERROR_RESULT;
    }
    out_file = fopen(outputFileName, "wb");
    if (!out_file) {
        LOGCATE("Could not open out file %s\n", inputFileName);
        return ERROR_RESULT;
    }

    AVOutputFormat *output = av_guess_format(NULL, outputFileName, NULL);
    LOGCATE("打印是否为null:%p",output);
    LOGCATE("print order for format:%d",output->video_codec);

    // 找到编码器
    codec = avcodec_find_encoder(output->video_codec);
    if (!codec) {
        LOGCATE("can't find coedc");
        return ERROR_RESULT;
    }


    codeCtx = avcodec_alloc_context3(codec);
    if (!codeCtx) {
        LOGCATE("Could not allocate video codec context\n");
        return ERROR_RESULT;
    }


    // 配置编码环境
//    codeCtx->codec_id = codec->id;
//    codeCtx->codec_type = AVMEDIA_TYPE_VIDEO;
    codeCtx->pix_fmt = AV_PIX_FMT_YUVJ420P;

    codeCtx->width = 840;
    codeCtx->height = 1074;

    codeCtx->time_base = AVRational{1, 25};
    codeCtx->framerate = AVRational{25, 1};
    codeCtx->bit_rate = 4000000;
//    codeCtx->max_b_frames = 10;
    codeCtx->gop_size = 1;

//    if (codec->id == AV_CODEC_ID_H264)
//        av_opt_set(codeCtx->priv_data, "preset", "slow", 0);

    // 打开编码器
    if ((ret = avcodec_open2(codeCtx, codec, NULL)) < 0) {
        LOGCATE("can't open encoder:%s",av_err2str(ret));
        return ERROR_RESULT;
    }
    LOGCATE("already open decoder");
    frame = av_frame_alloc();
    pkt = av_packet_alloc();
    frame->width = codeCtx->width;
    frame->height = codeCtx->height;
    frame->format = codeCtx->pix_fmt;
    int pictureSize = av_image_get_buffer_size(codeCtx->pix_fmt, codeCtx->width, codeCtx->height,
                                               1);
    ret = av_frame_get_buffer(frame, 1);
    if (ret < 0) {
        LOGCATE("can't get frame buffer");
        return ERROR_RESULT;
    }
    pic_buf = static_cast<uint8_t *>(av_malloc(pictureSize));
    int y_size = codeCtx->width * codeCtx->height;

    LOGCATE("prepare read yuv data");
    //read yuv
    if (fread(pic_buf, 1, pictureSize, in_file) <= 0) {
        LOGCATE("can't read yuv data");
        return ERROR_RESULT;
    }
    frame->data[0] = pic_buf;              // Y
    frame->data[1] = pic_buf + y_size;      // U
    frame->data[2] = pic_buf + y_size * 5 / 4;  // V

    //prepare read data
    encodeInternal(codeCtx, frame, pkt, out_file, NULL);

    encodeInternal(codeCtx, NULL, pkt, out_file, nullptr);

    /* add sequence end code to have a real MPEG file */
    fwrite(endcode, 1, sizeof(endcode), out_file);

    fclose(in_file);
    fclose(out_file);

    avcodec_free_context(&codeCtx);
    av_frame_free(&frame);
    av_packet_free(&pkt);


    LOGCATE("enough");
    finalResult = outputFileName;
    return finalResult;

}


void EncodeYuvToJpg::encodeInternal(AVCodecContext *pContext, AVFrame *pFrame, AVPacket *pPacket,
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

        LOGCATE("Write packet size:%d pktData:%p", pPacket->size,pPacket->data);
        fwrite(pPacket->data, 1, pPacket->size, pFile);
        av_packet_unref(pPacket);
    }
}
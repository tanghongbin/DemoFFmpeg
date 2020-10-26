//
// Created by Admin on 2020/10/16.
//


#include <utils.h>

#include "EncodeYuvToJpg.h"

extern "C" {
#include <libavutil/imgutils.h>
#include <libavcodec/avcodec.h>
#include <libavutil/opt.h>
#include <libavformat/avformat.h>
}

const char *EncodeYuvToJpg::encode(const char *sss) {
    const char *ERROR_RESULT = "";
    const char *inputFileName = "/storage/emulated/0/ffmpegtest/YUV_Image_840x1074.NV21";
    const char *finalResult;
    const AVCodec *codec;
    AVCodecContext *codeCtx = NULL;
    AVFormatContext *avFormatContext;
    AVOutputFormat *outputFormat;
    int i, ret, x, y;
    FILE *in_file;
    FILE *out_file;
    AVFrame *frame;
    AVPacket *pkt;
    AVStream *stream;
    uint8_t *pic_buf;
    uint8_t endcode[] = {0, 0, 1, 0xb7};

    const char *outputFileName = "/storage/emulated/0/ffmpegtest/test_out_file.jpg";

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

    avFormatContext = avformat_alloc_context();

//    //2.打开文件解封装
//    if (avformat_open_input(&avFormatContext, inputFileName, NULL, NULL) != 0) {
//        LOGCATE("init format error");
//        return ERROR_RESULT;
//    }
//
//    //3.获取音视频流信息
//    if (avformat_find_stream_info(avFormatContext, NULL) < 0) {
//        LOGCATE("can't find valid stream info");
//        return ERROR_RESULT;
//    }
//
////    ================
//avFormatContext->iformat;

    outputFormat = av_guess_format("mjpeg", NULL, NULL);
    if (!outputFormat) {
        LOGCATE("Could not create output format");
        return ERROR_RESULT;
    }
    avFormatContext->oformat = outputFormat;
    // open outfile
    if (avio_open(&avFormatContext->pb, outputFileName, AVIO_FLAG_READ_WRITE) < 0) {
        LOGCATE("Could not open input file %s\n", outputFileName);
        return ERROR_RESULT;
    }

    stream = avformat_new_stream(avFormatContext, 0);
    if (stream == nullptr) {
        LOGCATE("can't create new stream");
        return ERROR_RESULT;
    }

    // 找到编码器
    codec = avcodec_find_encoder(outputFormat->video_codec);
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
    codeCtx->codec_id = codec->id;
    codeCtx->codec_type = AVMEDIA_TYPE_VIDEO;
    codeCtx->pix_fmt = AV_PIX_FMT_YUVJ420P;

    codeCtx->width = 840;
    codeCtx->height = 1074;

    codeCtx->time_base.num = 1;
    codeCtx->time_base.den = 25;

    av_dump_format(avFormatContext,0,outputFileName,1);



    // 打开编码器
    if (avcodec_open2(codeCtx, codec, NULL) < 0) {
        LOGCATE("can't open encoder");
        return ERROR_RESULT;
    }
    LOGCATE("already open decoder");
    frame = av_frame_alloc();
    pkt = av_packet_alloc();
    int pictureSize = avpicture_get_size(codeCtx->pix_fmt, codeCtx->width, codeCtx->height);
    LOGCATE("already get picsize:%d",pictureSize);
    pic_buf = static_cast<uint8_t *>(av_malloc(pictureSize));
    if (!pic_buf) {
        LOGCATE("can't malloc picbuf");
        return ERROR_RESULT;
    }
    LOGCATE("picbuf get picsize alloc success");
    avformat_write_header(avFormatContext, NULL);
    int y_size = codeCtx->width * codeCtx->height;
    LOGCATE("prepare new packet,pring pkg %p",pkt);
    av_new_packet(pkt, y_size);
    LOGCATE("prepare read yuv data");
    //read yuv
    if (fread(pic_buf, 1, y_size * 3 / 2, in_file) <= 0) {
        LOGCATE("can't read yuv data");
        return ERROR_RESULT;
    }
    frame->data[0] = pic_buf;              // Y
    frame->data[1] = pic_buf + y_size;      // U
    frame->data[2] = pic_buf + y_size * 5 / 4;  // V

    //prepare read data
    encodeInternal(codeCtx,frame,pkt,out_file);

    encodeInternal(codeCtx, NULL, pkt, out_file);

    av_write_trailer(avFormatContext);
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


void EncodeYuvToJpg::encodeInternal(AVCodecContext *enc_ctx, AVFrame *frame, AVPacket *pkt,
                                    FILE *outfile) {

    int ret;

    /* send the frame to the encoder */
//    if (frame)
//        LOGCATE("Send frame %3"PRId64"\n", frame->pts);

    ret = avcodec_send_frame(enc_ctx, frame);
    if (ret < 0) {
        LOGCATE("Error sending a frame for encoding\n");
        return;
    }

    while (ret >= 0) {
        ret = avcodec_receive_packet(enc_ctx, pkt);
        if (ret == AVERROR(EAGAIN) || ret == AVERROR_EOF){
            LOGCATE("Error during encoding,encode has end");
            return;
        }

        else if (ret < 0) {
            LOGCATE("Error during encoding\n");
            return;
        }

        LOGCATE("Write packet size:%d", pkt->size);
        fwrite(pkt->data, 1, pkt->size, outfile);
        av_packet_unref(pkt);
    }
}
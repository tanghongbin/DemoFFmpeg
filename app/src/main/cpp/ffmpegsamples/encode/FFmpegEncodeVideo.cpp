//
// Created by Admin on 2020/10/26.
//

extern "C" {
#include <libavformat/avformat.h>
#include <libavutil/opt.h>
#include <libavutil/imgutils.h>
}

#include <utils.h>
#include <CustomGLUtils.h>
//#include <CustomGLUtils.h>


#include "FFmpegEncodeVideo.h"

FFmpegEncodeVideo *FFmpegEncodeVideo::instance = nullptr;

FFmpegEncodeVideo *FFmpegEncodeVideo::getInstance() {

    if (instance == nullptr) {
        instance = new FFmpegEncodeVideo;
        LOGCATE("create encode video instance");
    }
    return instance;

}

FFmpegEncodeVideo::FFmpegEncodeVideo() {
}

FFmpegEncodeVideo::~FFmpegEncodeVideo() {

}

void FFmpegEncodeVideo::destroyInstance() {

    if (instance) {
        delete instance;
        instance = nullptr;
    }
}

void FFmpegEncodeVideo::init() {


    LOGCATE("begin init demo");


    out_file = fopen(OUT_PUT_VIDEO_PATH, "wb");
    if (!out_file) {
        LOGCATE("Could not open out file %s\n", OUT_PUT_VIDEO_PATH);
        return;
    }


    /* find the mpeg1video encoder */
    codec = avcodec_find_encoder(AV_CODEC_ID_H264);
    if (!codec) {
        LOGCATE("Codec not found");
        exit(1);
    }

    codeCtx = avcodec_alloc_context3(codec);
    if (!codeCtx) {
        LOGCATE("Could not allocate video codec context\n");
        exit(1);
    }

    /* put sample parameters */
    codeCtx->bit_rate = 4000000;
    /* resolution must be a multiple of two */
    codeCtx->width = 840;
    codeCtx->height = 1074;
    /* frames per second */
    codeCtx->time_base = (AVRational) {1, 25};
    codeCtx->framerate = (AVRational) {25, 1};

    LOGCATE("log width:%d height:%d",mWindow_width,mWindow_height);

    /* emit one intra frame every ten frames
     * check frame pict_type before passing frame
     * to encoder, if frame->pict_type is AV_PICTURE_TYPE_I
     * then gop_size is ignored and the output of encoder
     * will always be I frame irrespective to gop_size
     */
    codeCtx->gop_size = 10;
    codeCtx->max_b_frames = 1;
    codeCtx->pix_fmt = AV_PIX_FMT_YUV420P;

    if (codec->id == AV_CODEC_ID_H264)
        av_opt_set(codeCtx->priv_data, "preset", "slow", 0);

    /* open it */
    ret = avcodec_open2(codeCtx, codec, NULL);
    if (ret < 0) {
        LOGCATE("Could not open codec: %s\n", av_err2str(ret));
        exit(1);
    }

    pkt = av_packet_alloc();
    if (!pkt)
        exit(1);

    frame = av_frame_alloc();
    if (!frame) {
        LOGCATE("Could not allocate video frame\n");
        exit(1);
    }
    frame->format = codeCtx->pix_fmt;
    frame->width = codeCtx->width;
    frame->height = codeCtx->height;

    ret = av_frame_get_buffer(frame, 1);
    if (checkNegativeReturn(ret, "av_frame_get_buffer error :%s")) return;

    int pic_size = av_image_get_buffer_size(codeCtx->pix_fmt, codeCtx->width, codeCtx->height, 1);


    pic_buf = static_cast<uint8_t *>(av_malloc(pic_size));
    LOGCATE("print pic_size:%d", pic_size);

//    av_new_packet(pkt, pic_size);
    if (ret < 0) {
        LOGCATE("Could not allocate the video frame data\n");
        exit(1);
    }

    int y_size = codeCtx->width * codeCtx->height;
    LOGCATE("print compute_size:%d real_size:%d", pic_size, y_size * 3 / 2);
//    ret = av_frame_make_writable(frame);
    /* encode 1 second of video */
//    for (i = 0; i < 250; i++) {
//        fflush(stdout);
//
//        /* make sure the frame data is writable */
//        ret = av_frame_make_writable(frame);
//        if (ret < 0)
//            exit(1);
//
//        /* prepare a dummy image */
//        /* Y */
//        for (y = 0; y < c->height; y++) {
//            for (x = 0; x < c->width; x++) {
//                frame->data[0][y * frame->linesize[0] + x] = x + y + i * 3;
//            }
//        }
//
//        /* Cb and Cr */
//        for (y = 0; y < c->height/2; y++) {
//            for (x = 0; x < c->width/2; x++) {
//                frame->data[1][y * frame->linesize[1] + x] = 128 + y + i * 2;
//                frame->data[2][y * frame->linesize[2] + x] = 64 + x + i * 5;
//            }
//        }
//
//        frame->pts = i;
//
//        /* encode the image */
//        encode(c, frame, pkt, f);
//    }



    End:



    mHasInitSuccess = true;
}


void FFmpegEncodeVideo::unInit() {
    if (!mHasInitSuccess)
        return;

    uint8_t endcode[] = {0, 0, 1, 0xb7};

    /* flush the encoder */
    encode(codeCtx, NULL, pkt, out_file);

    /* add sequence end code to have a real MPEG file */
    fwrite(endcode, 1, sizeof(endcode), out_file);

    av_freep(pic_buf);
    fclose(out_file);
    fclose(in_file);

    avcodec_free_context(&codeCtx);
    av_frame_free(&frame);
    av_packet_free(&pkt);
}

void FFmpegEncodeVideo::testReadFile() {

    int w = 840, h = 1074;
    int pic_size = w * h * 3 / 2;
    int y_size = w * h;
    uint8_t *pic_buf = reinterpret_cast<uint8_t *>(av_malloc(pic_size));
    const char *inputFileName = "/storage/emulated/0/ffmpegtest/YUV_Image_840x1074.NV21";
    FILE *in_file = fopen(inputFileName, "rb");
    if (!in_file) {
        LOGCATE("can't open inputfile");
        return;
    }
    LOGCATE("log read data before :%p", pic_buf);
    if (fread(pic_buf, 1, pic_size, in_file) <= 0) {
        LOGCATE("can't read yuv maybe error");
    } else if (feof(in_file)) {
        LOGCATE("can't read yuv maybe finished");
    } else {
        LOGCATE("read yuv maybe complete");
    }

    LOGCATE("log read data after :%p", pic_buf);
}


void FFmpegEncodeVideo::encodeVideoFrame(uint8_t *pic_src) {
    if (!mHasInitSuccess)
        return;
    int ret;

    NvToYUV420p(pic_src,pic_buf,codeCtx->width,codeCtx->height);
    LOGCATE("convert over :%p",pic_buf);
    int y_size = codeCtx->width * codeCtx->height;
    frame->data[0] = pic_buf;              // Y
    frame->data[1] = pic_buf + y_size;      // U
    frame->data[2] = pic_buf + y_size * 5 / 4;  // V
    frame->pts = ++mFrameIndex;
    ret = avcodec_send_frame(codeCtx, frame);
    if (ret < 0) {
        LOGCATE("sene frame failed or end");
        goto END;
    }
    ret = avcodec_receive_packet(codeCtx, pkt);
    if (ret < 0) {
        goto END;
    }
    fwrite(pkt->data, 1, pkt->size, out_file);

    END:
    av_packet_unref(pkt);
}


void
FFmpegEncodeVideo::encode(AVCodecContext *enc_ctx, AVFrame *frame, AVPacket *pkt, FILE *outfile) {
    int ret;

    /* send the frame to the encoder */
    if (frame)
        LOGCATE("Send frame:%jd", frame->pts);

    ret = avcodec_send_frame(enc_ctx, frame);
    if (ret < 0) {
        LOGCATE("Error sending a frame for encoding:%s", av_err2str(ret));
        return;
    }

    while (ret >= 0) {
        ret = avcodec_receive_packet(enc_ctx, pkt);
        if (ret == AVERROR(EAGAIN) || ret == AVERROR_EOF)
            return;
        else if (ret < 0) {
            LOGCATE("Error during encoding:%s", av_err2str(ret));
            return;
        }

        printf("Write packet ", pkt->pts, pkt->size);
        fwrite(pkt->data, 1, pkt->size, outfile);
        av_packet_unref(pkt);
    }
}

void FFmpegEncodeVideo::initOffcialDemo() {
    LOGCATE("begin init offcial demo");
    const AVCodec *codec;
    AVCodecContext *c = NULL;
    int ret;
    AVFrame *frame;
    AVPacket *pkt;
    uint8_t endcode[] = {0, 0, 1, 0xb7};

    const char *filename = "/storage/emulated/0/ffmpegtest/encodeVideo.h264";

    const char *inputFileName = "/storage/emulated/0/ffmpegtest/YUV_Image_840x1074.NV21";


    FILE *in_file;

    FILE *out_file = fopen(filename, "wb");
    if (!out_file) {
        LOGCATE("Could not open out file %s\n", filename);
        return;
    }


    /* find the mpeg1video encoder */
    codec = avcodec_find_encoder(AV_CODEC_ID_H264);
    if (!codec) {
        LOGCATE("Codec not found");
        exit(1);
    }

    c = avcodec_alloc_context3(codec);
    if (!c) {
        LOGCATE("Could not allocate video codec context\n");
        exit(1);
    }

    pkt = av_packet_alloc();
    if (!pkt)
        exit(1);

    /* put sample parameters */
    c->bit_rate = 400000;
    /* resolution must be a multiple of two */
    c->width = 840;
    c->height = 1074;
    /* frames per second */
    c->time_base = (AVRational) {1, 25};
    c->framerate = (AVRational) {25, 1};

    /* emit one intra frame every ten frames
     * check frame pict_type before passing frame
     * to encoder, if frame->pict_type is AV_PICTURE_TYPE_I
     * then gop_size is ignored and the output of encoder
     * will always be I frame irrespective to gop_size
     */
    c->gop_size = 10;
    c->max_b_frames = 1;
    c->pix_fmt = AV_PIX_FMT_YUV420P;

    if (codec->id == AV_CODEC_ID_H264)
        av_opt_set(c->priv_data, "preset", "slow", 0);

    /* open it */
    ret = avcodec_open2(c, codec, NULL);
    if (ret < 0) {
        LOGCATE("Could not open codec: %s\n", av_err2str(ret));
        exit(1);
    }

    frame = av_frame_alloc();
    if (!frame) {
        LOGCATE("Could not allocate video frame\n");
        exit(1);
    }
    frame->format = c->pix_fmt;
    frame->width = c->width;
    frame->height = c->height;

    ret = av_frame_get_buffer(frame, 1);
    if (checkNegativeReturn(ret, "av_frame_get_buffer error :%s")) return;

    int pic_size = av_image_get_buffer_size(c->pix_fmt, c->width, c->height, 1);


    LOGCATE("print pic_size:%d", pic_size);
    uint8_t *pic_buf = static_cast<uint8_t *>(av_malloc(pic_size));

//    av_new_packet(pkt, pic_size);
    if (ret < 0) {
        LOGCATE("Could not allocate the video frame data\n");
        exit(1);
    }

    int y_size = c->width * c->height;
    LOGCATE("print compute_size:%d real_size:%d", pic_size, y_size * 3 / 2);
    ret = av_frame_make_writable(frame);
    if (checkNegativeReturn(ret, "frame is not writeable")) {
        exit(1);
    }
    int frameNum = 100;

    for (int ttt = 1; ttt < frameNum; ++ttt) {
        long long cur = GetSysCurrentTime();
        in_file = fopen(inputFileName, "rb");
        if (!in_file) {
            LOGCATE("Could not open input file %s\n", inputFileName);
            goto End;
        }
        if (fread(pic_buf, 1, pic_size, in_file) <= 0) {
            LOGCATE("can't read yuv maybe error");
            goto End;
        } else if (feof(in_file)) {
            LOGCATE("read yuv maybe finished");
            break;
        } else {
            LOGCATE("read yuv maybe complete");
        }
        frame->data[0] = pic_buf;              // Y
        frame->data[1] = pic_buf + y_size;      // U
        frame->data[2] = pic_buf + y_size * 5 / 4;  // V

        frame->pts = ttt;
        //prepare read data
        encode(c, frame, pkt, out_file);
        fclose(in_file);
        LOGCATE("encode video frame and open file spend time:%jd", GetSysCurrentTime() - cur);

    }
    /* encode 1 second of video */
//    for (i = 0; i < 250; i++) {
//        fflush(stdout);
//
//        /* make sure the frame data is writable */
//        ret = av_frame_make_writable(frame);
//        if (ret < 0)
//            exit(1);
//
//        /* prepare a dummy image */
//        /* Y */
//        for (y = 0; y < c->height; y++) {
//            for (x = 0; x < c->width; x++) {
//                frame->data[0][y * frame->linesize[0] + x] = x + y + i * 3;
//            }
//        }
//
//        /* Cb and Cr */
//        for (y = 0; y < c->height/2; y++) {
//            for (x = 0; x < c->width/2; x++) {
//                frame->data[1][y * frame->linesize[1] + x] = 128 + y + i * 2;
//                frame->data[2][y * frame->linesize[2] + x] = 64 + x + i * 5;
//            }
//        }
//
//        frame->pts = i;
//
//        /* encode the image */
//        encode(c, frame, pkt, f);
//    }



    End:

    /* flush the encoder */
    encode(c, NULL, pkt, out_file);

    /* add sequence end code to have a real MPEG file */
    fwrite(endcode, 1, sizeof(endcode), out_file);

    av_free(pic_buf);
    fclose(out_file);
    fclose(in_file);

    avcodec_free_context(&c);
    av_frame_free(&frame);
    av_packet_free(&pkt);
    LOGCATE("encode video finished");
}

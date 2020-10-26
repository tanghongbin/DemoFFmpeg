//
// Created by Admin on 2020/10/26.
//

extern "C" {
#include <libavformat/avformat.h>
#include <libavutil/opt.h>
}

#include <utils.h>
#include <CustomGLUtils.h>

#include "FFmpegEncodeVideo.h"

FFmpegEncodeVideo *FFmpegEncodeVideo::instance = nullptr;

FFmpegEncodeVideo *FFmpegEncodeVideo::getInstance() {

    if (instance == nullptr) {
        instance = new FFmpegEncodeVideo;
        LOGCATE("create encode video instance");
    }
    return instance;

}

void FFmpegEncodeVideo::destroyInstance() {

    if (instance) {
        delete instance;
        instance = nullptr;
    }
}

void FFmpegEncodeVideo::init() {

    int ret;

    const char *outFileName = "/storage/emulated/0/ffmpegtest/encodeVideo.h264";


    // 找到编码器
    mCodec = avcodec_find_encoder(AV_CODEC_ID_H264);
    if (!mCodec) {
        LOGCATE("can't find encoder");
        return;
    }
    mCodeCtx = avcodec_alloc_context3(mCodec);
    if (!mCodeCtx) {
        LOGCATE("can't alloc mCodeCtx");
        return;
    }
    mCodeCtx->codec_id = mCodec->id;
    mCodeCtx->codec_type = AVMEDIA_TYPE_VIDEO;
    mCodeCtx->pix_fmt = AV_PIX_FMT_YUV420P;

    mCodeCtx->width = 960;
    mCodeCtx->height = 544;
    mCodeCtx->time_base = AVRational{1, 25};
    mCodeCtx->framerate = AVRational{25, 1};

    mCodeCtx->bit_rate = 400000;
    // 暂时设置
    mCodeCtx->gop_size = 250;
    mCodeCtx->max_b_frames = 3;

    mCodeCtx->qmin = 10;
    mCodeCtx->qmax = 51;

    if (mCodec->id == AV_CODEC_ID_H264){
        av_opt_set(mCodeCtx->priv_data, "preset", "slow", 0);
        av_opt_set(mCodeCtx->priv_data, "tune", "zerolatency", 0);
    }

    ret = avcodec_open2(mCodeCtx, mCodec, NULL);
    if (checkNegativeReturn(ret, av_err2str(ret))) return;

    mOutFile = fopen(outFileName, "wb");

    if (!outFileName) {
        LOGCATE("can't open encode video file");
        return;
    }

    mAvFrame = av_frame_alloc();
    if (!mAvFrame) {
        LOGCATE("can't alloc mAvFrame");
        return;
    }
    mAvPacket = av_packet_alloc();
    if (!mAvPacket) {
        LOGCATE("can't alloc mAvPacket");
        return;
    }
    mAvFrame->format = mCodeCtx->pix_fmt;
    mAvFrame->width = mCodeCtx->width;
    mAvFrame->height = mCodeCtx->height;
    ret = av_frame_get_buffer(mAvFrame, 1);
    if (checkNegativeReturn(ret, "mAvFrame get buffer failed")) return;
    ret = av_frame_make_writable(mAvFrame);
    if (checkNegativeReturn(ret, "mAvFrame is not writable")) return;

    mHasInitSuccess = true;
}


void FFmpegEncodeVideo::unInit() {
    if (!mHasInitSuccess)
        return;
    fclose(mOutFile);
    av_packet_unref(mAvPacket);
    avcodec_free_context(&mCodeCtx);
    av_frame_free(&mAvFrame);
    av_packet_free(&mAvPacket);
}

void FFmpegEncodeVideo::encodeVideoFrame(uint8_t *p) {
    if (!mHasInitSuccess)
        return;
    int ret;
    mAvFrame->data[0] = p;
    mAvFrame->pts = ++mFrameIndex;
    ret = avcodec_send_frame(mCodeCtx, mAvFrame);
    if (ret < 0) {
        LOGCATE("sene frame failed or end");
        goto END;
    }
    ret = avcodec_receive_packet(mCodeCtx, mAvPacket);
    if (ret < 0) {
        goto END;
    }
    fwrite(mAvPacket->data, 1, mAvPacket->size, mOutFile);

    END:
    av_packet_unref(mAvPacket);
}

void FFmpegEncodeVideo::initOffcialDemo() {
    LOGCATE("begin init offcial demo");
    const AVCodec *codec;
    AVCodecContext *c= NULL;
    int i, ret, x, y;
    FILE *f;
    AVFrame *frame;
    AVPacket *pkt;
    uint8_t endcode[] = { 0, 0, 1, 0xb7 };

    const char *filename = "/storage/emulated/0/ffmpegtest/encodeVideo.h264";


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
    c->width = 352;
    c->height = 288;
    /* frames per second */
    c->time_base = (AVRational){1, 25};
    c->framerate = (AVRational){25, 1};

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

    f = fopen(filename, "wb");
    if (!f) {
        LOGCATE("Could not open %s\n", filename);
        exit(1);
    }

    frame = av_frame_alloc();
    if (!frame) {
        LOGCATE("Could not allocate video frame\n");
        exit(1);
    }
    frame->format = c->pix_fmt;
    frame->width  = c->width;
    frame->height = c->height;

    ret = av_frame_get_buffer(frame, 32);
    if (ret < 0) {
        LOGCATE("Could not allocate the video frame data\n");
        exit(1);
    }

    /* encode 1 second of video */
    for (i = 0; i < 25; i++) {
        fflush(stdout);

        /* make sure the frame data is writable */
        ret = av_frame_make_writable(frame);
        if (ret < 0)
            exit(1);

        /* prepare a dummy image */
        /* Y */
        for (y = 0; y < c->height; y++) {
            for (x = 0; x < c->width; x++) {
                frame->data[0][y * frame->linesize[0] + x] = x + y + i * 3;
            }
        }

        /* Cb and Cr */
        for (y = 0; y < c->height/2; y++) {
            for (x = 0; x < c->width/2; x++) {
                frame->data[1][y * frame->linesize[1] + x] = 128 + y + i * 2;
                frame->data[2][y * frame->linesize[2] + x] = 64 + x + i * 5;
            }
        }

        frame->pts = i;

        /* encode the image */
        encode(c, frame, pkt, f);
    }

    /* flush the encoder */
    encode(c, NULL, pkt, f);

    /* add sequence end code to have a real MPEG file */
    fwrite(endcode, 1, sizeof(endcode), f);
    fclose(f);

    avcodec_free_context(&c);
    av_frame_free(&frame);
    av_packet_free(&pkt);
}

void
FFmpegEncodeVideo::encode(AVCodecContext *enc_ctx, AVFrame* frame, AVPacket *pkt, FILE *outfile) {
    int ret;

    /* send the frame to the encoder */
    if (frame)
        printf("Send frame", frame->pts);

    ret = avcodec_send_frame(enc_ctx, frame);
    if (ret < 0) {
        LOGCATE("Error sending a frame for encoding\n");
        exit(1);
    }

    while (ret >= 0) {
        ret = avcodec_receive_packet(enc_ctx, pkt);
        if (ret == AVERROR(EAGAIN) || ret == AVERROR_EOF)
            return;
        else if (ret < 0) {
            LOGCATE("Error during encoding\n");
            exit(1);
        }

        printf("Write packet ", pkt->pts, pkt->size);
        fwrite(pkt->data, 1, pkt->size, outfile);
        av_packet_unref(pkt);
    }
}

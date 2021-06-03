//
// Created by Admin on 2020/10/26.
//








#include <encoder/FFmpegEncodeVideo.h>
#include <utils/utils.h>
#include <utils/CustomGLUtils.h>
#include <libavutil/imgutils.h>

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

    if (mHasInitSuccess) {
        LOGCATE("has init success, don't need to reinit");
        return;
    }

    LOGCATE("begin init FFmpegEncodeVideo");

    const char *out_file_name = getRandomStr("encodevideo_", ".mp4", "encodeVideos/");


    ret = avformat_alloc_output_context2(&ofmtctx, NULL, NULL, out_file_name);
    AVOutputFormat *ofmt = ofmtctx->oformat;
    if (ret < 0) {
        LOGCATE("avformat_alloc_output_context2 failed:%s", av_err2str(ret));
        return;
    }
    /* find the mpeg1video encoder */
    codec = avcodec_find_encoder(ofmt->video_codec);
    if (!codec) {
        LOGCATE("Codec not found");
        exit(1);
    }

    codeCtx = avcodec_alloc_context3(codec);
    if (!codeCtx) {
        LOGCATE("Could not allocate video codec context\n");
        exit(1);
    }

    codeCtx->codec_id = codec->id;
    codeCtx->codec_type = AVMEDIA_TYPE_VIDEO;
    /* put sample parameters */
    codeCtx->bit_rate = 3500 * 1000;
    /* resolution must be a multiple of two */
    codeCtx->width = mWindow_height;
    codeCtx->height = mWindow_width;
    /* frames per second */
    codeCtx->time_base = (AVRational) {1, 25};
    codeCtx->framerate = (AVRational) {25, 1};
    LOGCATE("log width:%d height:%d", codeCtx->width, codeCtx->height);
    codeCtx->gop_size = 1;
    codeCtx->max_b_frames = 1;
    codeCtx->pix_fmt = AV_PIX_FMT_YUV420P;

    AVDictionary *opt = 0;
    if (codec->id == AV_CODEC_ID_H264) {
        av_dict_set_int(&opt, "video_track_timescale", 25, 0);
        av_dict_set(&opt, "preset", "veryfast", 0);
        av_dict_set(&opt, "tune", "zerolatency", 0);
    }

    // create output ctx
    oStream = avformat_new_stream(ofmtctx, NULL);
    ret = avcodec_parameters_from_context(oStream->codecpar, codeCtx);
    if (ret < 0) {
        LOGCATE("avcodec_parameters_from_context failed:%s", av_err2str(ret));
        return;
    }
    oStream->codecpar->codec_tag = 0;
    if (!(ofmtctx->flags & AVFMT_NOFILE)) {
        if ((ret = avio_open(&ofmtctx->pb, out_file_name, AVIO_FLAG_WRITE)) < 0) {
            LOGCATE("avio_open failed:%s", av_err2str(ret));
            return;
        }
    }

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


    if (checkNegativeReturn(ret, "av_frame_get_buffer error :%s")) return;

    bufferSize = av_image_get_buffer_size(codeCtx->pix_fmt, codeCtx->width, codeCtx->height, 1);
//    int lineSize = av_image_get_linesize(codeCtx->pix_fmt, codeCtx->width, 1);
//    pic_buf = static_cast<uint8_t *>(av_malloc(bufferSize));
    LOGCATE("print pic_size:%d fmt:%d width:%d height:%d", bufferSize, codeCtx->pix_fmt,
            codeCtx->width, codeCtx->height);
//    ret = av_image_fill_arrays(frame->data,frame->linesize,pic_buf,codeCtx->pix_fmt,codeCtx->width,codeCtx->height,1);

    ret = av_frame_get_buffer(frame, 1);
    if (ret < 0) {
        fprintf(stderr, "Could not allocate the video frame data\n");
        exit(1);
    }

    if (ret < 0) {
        LOGCATE("Could not allocate the video frame data\n");
        exit(1);
    }

    int y_size = codeCtx->width * codeCtx->height;
    LOGCATE("print compute_size:%d real_size:%d", bufferSize, y_size * 3 / 2);

    ret = avformat_write_header(ofmtctx, &opt);
    if (ret < 0) {
        LOGCATE("avformat_write_header failed :%s", av_err2str(ret));
        return;
    }

    mHasInitSuccess = true;
    LOGCATE("video encode init success ");

    av_dump_format(ofmtctx, 0, out_file_name, 1);
}


void FFmpegEncodeVideo::unInit() {
    if (!mHasInitSuccess)
        return;


    av_write_trailer(ofmtctx);
    avcodec_free_context(&codeCtx);
    av_frame_free(&frame);
    av_packet_free(&pkt);

    if (ofmtctx && !(ofmtctx->flags & AVFMT_NOFILE)) {
        avio_close(ofmtctx->pb);
    }

    if (ofmtctx) {
        avformat_free_context(ofmtctx);
    }

    ofmtctx = nullptr;
    codeCtx = nullptr;
    frame = nullptr;
    pkt = nullptr;

}


void FFmpegEncodeVideo::encodeVideoFrame(uint8_t *pic_src) {
    if (!mHasInitSuccess)
        return;

    int ret;
    int y_size = codeCtx->width * codeCtx->height;
    double duration = AV_TIME_BASE / av_q2d(codeCtx->framerate);
    ret = av_frame_make_writable(frame);
    if (ret < 0) {
        LOGCATE("av_frame_make_writable failed: code:%d  errorStr:%s", ret, av_err2str(ret));
        return;
    }
    frame->data[0] = pic_src;              // Y
    frame->data[1] = pic_src + y_size;      // U
    frame->data[2] = pic_src + y_size * 5 / 4;  // V
    ret = avcodec_send_frame(codeCtx, frame);
    if (ret < 0) {
        LOGCATE("avcodec_send_frame failed:%s", av_err2str(ret));
        return;
    }

    while (ret >= 0) {
        ret = avcodec_receive_packet(codeCtx, pkt);
        LOGCATE("receive packet:%s", av_err2str(ret));
        if (ret == AVERROR(EAGAIN) || ret == AVERROR_EOF) {
            break;
        } else if (ret < 0) {
            LOGCATE("Error during encoding:%s", av_err2str(ret));
            exit(1);
        }

        // 设置时间戳
        AVRational time_base = ofmtctx->streams[oStream->index]->time_base;
        int64_t everyFrameDuration = (double)AV_TIME_BASE / av_q2d(time_base);
        pkt->pts = (double) (pts_frame_index * everyFrameDuration) / (av_q2d(time_base)*AV_TIME_BASE);
        pkt->dts = pkt->pts;
        pkt->duration = (double)everyFrameDuration/(double)(av_q2d(time_base)*AV_TIME_BASE);

        pkt->stream_index = oStream->index;
        av_packet_rescale_ts(pkt, codeCtx->time_base, oStream->time_base);
        pkt->pos = -1;
        ret = av_interleaved_write_frame(ofmtctx, pkt);
        if (ret < 0) {
            LOGCATE("av_interleaved_write_frame failed:%s", av_err2str(ret));
            exit(1);
        }
        av_packet_unref(pkt);
    }

//    END:
//    av_packet_unref(pkt);
}



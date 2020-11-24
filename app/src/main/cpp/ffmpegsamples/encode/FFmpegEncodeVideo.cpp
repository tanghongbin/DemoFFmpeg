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

    const char * out_file_name = getRandomStr("encodevideo_",".h264","encodeVideos/");


    ret = avformat_alloc_output_context2(&ofmtctx,NULL,NULL,out_file_name);
    AVOutputFormat *ofmt = ofmtctx->oformat;
    if (ret < 0){
        LOGCATE("avformat_alloc_output_context2 failed:%s",av_err2str(ret));
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

    /* put sample parameters */
    codeCtx->bit_rate = 3500 *1000;
    /* resolution must be a multiple of two */
    codeCtx->width = mWindow_width;
    codeCtx->height = mWindow_height;
    /* frames per second */
    codeCtx->time_base = (AVRational) {1, 25};
    codeCtx->framerate = (AVRational) {25, 1};

    LOGCATE("log width:%d height:%d",codeCtx->width,codeCtx->height);

    /* emit one intra frame every ten frames
     * check frame pict_type before passing frame
     * to encoder, if frame->pict_type is AV_PICTURE_TYPE_I
     * then gop_size is ignored and the output of encoder
     * will always be I frame irrespective to gop_size
     */
    codeCtx->gop_size = 25;
    codeCtx->max_b_frames = 0;
    codeCtx->pix_fmt = AV_PIX_FMT_YUV420P;

    if (codec->id == AV_CODEC_ID_H264)
        av_opt_set(codeCtx->priv_data, "preset", "veryfast", 0);

    // create output ctx
    oStream = avformat_new_stream(ofmtctx,NULL);
    ret = avcodec_parameters_from_context(oStream->codecpar,codeCtx);
    if (ret < 0){
        LOGCATE("avcodec_parameters_from_context failed:%s",av_err2str(ret));
        return;
    }
    oStream->codecpar->codec_tag = 0;
    if (!(ofmtctx->flags & AVFMT_NOFILE)){
        if((ret = avio_open(&ofmtctx->pb,out_file_name,AVIO_FLAG_WRITE)) < 0){
            LOGCATE("avio_open failed:%s",av_err2str(ret));
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

    bufferSize = av_image_get_buffer_size(codeCtx->pix_fmt, codeCtx->width, codeCtx->height, 32);
//    int lineSize = av_image_get_linesize(codeCtx->pix_fmt, codeCtx->width, 1);
//    pic_buf = static_cast<uint8_t *>(av_malloc(bufferSize));
    LOGCATE("print pic_size:%d fmt:%d width:%d height:%d", bufferSize,codeCtx->pix_fmt,codeCtx->width,codeCtx->height);
//    ret = av_image_fill_arrays(frame->data,frame->linesize,pic_buf,codeCtx->pix_fmt,codeCtx->width,codeCtx->height,1);

    ret = av_frame_get_buffer(frame, 32);
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

    ret = avformat_write_header(ofmtctx,NULL);
    if (ret < 0) {
        LOGCATE("avformat_write_header failed :%s",av_err2str(ret));
        return;
    }

    mHasInitSuccess = true;
    LOGCATE("video encode init success ");
}


void FFmpegEncodeVideo::unInit() {
    if (!mHasInitSuccess)
        return;
    

    av_write_trailer(ofmtctx);
//    av_freep(pic_buf);
    fclose(in_file);

    avcodec_free_context(&codeCtx);
    av_frame_free(&frame);
    av_packet_free(&pkt);

    if (ofmtctx && !(ofmtctx->flags & AVFMT_NOFILE)){
        avio_close(ofmtctx->pb);
    }

    if (ofmtctx){
        avformat_free_context(ofmtctx);
    }

}



void FFmpegEncodeVideo::encodeVideoFrame(uint8_t *pic_src) {
    if (!mHasInitSuccess)
        return;
    int ret;
    uint8_t temp_array[bufferSize];
    int y_size = codeCtx->width * codeCtx->height;
    double duration = AV_TIME_BASE / av_q2d(codeCtx->framerate);
    ret = av_frame_make_writable(frame);
    if (ret < 0){
        LOGCATE("av_frame_make_writable failed: code:%d  errorStr:%s",ret,av_err2str(ret));
        goto END;
    }
    NvToYUV420p(pic_src,temp_array,codeCtx->width,codeCtx->height);
    LOGCATE("convert over :%p",temp_array);
    frame->data[0] = temp_array;              // Y
    frame->data[1] = temp_array + y_size;      // U
    frame->data[2] = temp_array + y_size * 5 / 4;  // V
//    double pts = (double)(pts_frame_index / (av_q2d(codeCtx->framerate))) * AV_TIME_BASE;



//    LOGCATE("current pts:%f",pts);
    frame->pts = pts_frame_index;
    ret = avcodec_send_frame(codeCtx, frame);
    if (ret < 0) {
        LOGCATE("avcodec_send_frame failed:%s",av_err2str(ret));
        goto END;
    }
    ret = avcodec_receive_packet(codeCtx, pkt);
    if (ret < 0) {
        LOGCATE("avcodec_receive_packet failed code:%d errorStr:%s",ret,av_err2str(ret));
        goto END;
    }
//    pkt->dts = pts;
    pkt->stream_index = 0;
    ret = av_write_frame(ofmtctx,pkt);
    if (ret < 0) {
        LOGCATE("av_interleaved_write_frame failed:%s",av_err2str(ret));
        goto END;
    }

    pts_frame_index++;
    dts_frame_index++;

    END:
    av_packet_unref(pkt);
}



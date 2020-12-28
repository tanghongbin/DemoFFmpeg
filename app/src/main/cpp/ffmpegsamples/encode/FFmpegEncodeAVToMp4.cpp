//
// Created by Admin on 2020/11/30.
//

#include <utils.h>
#include <CustomGLUtils.h>
#include <capturer/AudioRecordPlayHelper.h>



#include "FFmpegEncodeAVToMp4.h"

extern "C"{
#include <libavutil/timestamp.h>
#include <libavutil/time.h>
#include <libavutil/opt.h>
#include <libavutil/imgutils.h>
}

FFmpegEncodeAVToMp4* FFmpegEncodeAVToMp4::instance = nullptr;

FFmpegEncodeAVToMp4* FFmpegEncodeAVToMp4::getInstance(){
    if(instance == nullptr){
        instance = new FFmpegEncodeAVToMp4;
    }
    return instance;
}

void FFmpegEncodeAVToMp4::destroyInstance(){
    instance = nullptr;
    LOGCATE("has destroyed");
}

void FFmpegEncodeAVToMp4::startRecordMp4(){
    LOGCATE("start record mp4");
    isRecording = true;
    AudioRecordPlayHelper::getInstance()->notify_weak();
}

void FFmpegEncodeAVToMp4::stopRecordMp4(){
    LOGCATE("stop record mp4");
    isRecording = false;
}

void FFmpegEncodeAVToMp4::initAvEncoder(){
    // 先初始化视频的，再初始化音频的
    if (mHasInitSuccess) {
        LOGCATE("has init success, don't need to reinit");
        return;
    }

//    out_file_name = getRandomStr("encodevideo_", ".mp4", "encodemuxermp4/");
    out_file_name = RTMP_PUSH_URL;


    ret = initEncodeAudio();
    if (checkNegativeReturn(ret,"init audio error"))
        return;

    ret = initEncodeVideo();
    if (checkNegativeReturn(ret,"init video error"))
        return;



    mHasInitSuccess = true;
    LOGCATE("video encode init success ");

    av_dump_format(ofmtctx, 0, out_file_name, 1);

    audioCaptureThread = new std::thread(startCaptureAudio,this);
}

void FFmpegEncodeAVToMp4::startCaptureAudio(FFmpegEncodeAVToMp4* instance){
    AudioRecordPlayHelper::getInstance()->startCapture(instance);
}

void FFmpegEncodeAVToMp4::unInitAvEncoder(){
    if (!mHasInitSuccess)
        return;
    isRecording = false;
    encode_frame_Av(nullptr,0,1);
    encode_frame_Av(nullptr,0,2);
    LOGCATE("start unInit");
    // 视频
    av_write_trailer(ofmtctx);
    avcodec_free_context(&codeCtxV);
    av_frame_free(&frameV);
    av_packet_free(&pktV);

    if (ofmtctx && !(ofmtctx->flags & AVFMT_NOFILE)) {
        avio_close(ofmtctx->pb);
    }

    AudioRecordPlayHelper::getInstance()->stopCapture();
    AudioRecordPlayHelper::getInstance()->notify_weak();
    audioCaptureThread->join();
    delete audioCaptureThread;
    audioCaptureThread = nullptr;
    AudioRecordPlayHelper::destroyInstance();

    if (ofmtctx) {
        avformat_free_context(ofmtctx);
    }
    LOGCATE("start unInit middle");
    ofmtctx = nullptr;
    codeCtxV = nullptr;
    frameV = nullptr;
    pktV = nullptr;


    // 音频
    av_free(frame_bufferA);
    av_packet_unref(packetA);
    av_packet_free(&packetA);
    av_frame_free(&frameA);
    swr_free(&swr);

    avcodec_free_context(&codeCtxA);

    LOGCATE("start unInit over");
}

// 1-音频，2-视频
void FFmpegEncodeAVToMp4::encode_frame_Av(uint8_t* buffer, int length, int mediaType){

    if (!mHasInitSuccess)
        return;
    if (!isRecording)
        return;

    if (mediaType == 1){
        uint8_t *out[2];
        out[0] = new uint8_t[length];
        out[1] = new uint8_t[length];

        const uint8_t * convert_buffer = buffer;
        ret = swr_convert(swr,out,length * 4,&convert_buffer,length/4);
        if (ret < 0){
            LOGCATE("convert error:%s",av_err2str(ret));
            goto EndEncode;
        }
        frameA->data[0] = out[0];
        frameA->data[1] = out[1];

        ret = avcodec_send_frame(codeCtxA, frameA);
        if (ret < 0) {
            LOGCATE("avcodec_send_frame failed :%s",av_err2str(ret));
            goto EndEncode;
        }
        while (ret >= 0) {
            ret = avcodec_receive_packet(codeCtxA, packetA);
            if (ret == AVERROR(EAGAIN) || ret == AVERROR_EOF) {
                break;
            } else if (ret < 0) {
                LOGCATE("avcodec_receive_packet failed :%s",av_err2str(ret));
                goto EndEncode;
            }
            if (packetA->pts == AV_NOPTS_VALUE){
//                LOGCATE("no pts value");
            }
            if (packetA->dts == AV_NOPTS_VALUE){
//                LOGCATE("no dts value");
//                packetA->dts = packetA->pts;
            }
            packetA->stream_index = oStreamA->index;

            ret = av_interleaved_write_frame(ofmtctx,packetA);
//            logPkt(ofmtctx,packetA);
            if (ret < 0){
                LOGCATE("av_interleaved_write_frame failed :%s",av_err2str(ret));
                goto EndEncode;
            }
            av_packet_unref(packetA);
        }

        EndEncode:
        av_packet_unref(packetA);
        delete out[0];
        delete out[1];
    } else {

        int ret;
        int y_size = codeCtxV->width * codeCtxV->height;
        LOGCATE("encode width:%d height:%d", codeCtxV->width, codeCtxV->height);
        ret = av_frame_make_writable(frameV);
        if (ret < 0) {
            LOGCATE("av_frame_make_writable failed: code:%d  errorStr:%s", ret, av_err2str(ret));
            return;
        }
        frameV->data[0] = buffer;              // Y
        frameV->data[1] = buffer + y_size;      // U
        frameV->data[2] = buffer + y_size * 5 / 4;  // V

        AVRational time_base1 = codeCtxV->time_base;
        //Duration between 2 frames (us)
        int64_t calc_duration =
                (double) AV_TIME_BASE / av_q2d(codeCtxV->framerate);
        //Parameters
        frameV->pkt_duration = calc_duration;
        frameV->pts = pts_frame_indexV;

        LOGCATE("frame -> pts = %lld  duration:%lld",frameV->pts,calc_duration);
        ret = avcodec_send_frame(codeCtxV, frameV);
        if (ret < 0) {
            LOGCATE("avcodec_send_frame failed:%s", av_err2str(ret));
            return;
        }

        while (ret >= 0) {
            ret = avcodec_receive_packet(codeCtxV, pktV);
            LOGCATE("receive packet:%s", av_err2str(ret));
            if (ret == AVERROR(EAGAIN) || ret == AVERROR_EOF) {
                LOGCATE("receive pkt AVERROR or AVERROR_EOF");
                break;
            } else if (ret < 0) {
                LOGCATE("Error during encoding:%s", av_err2str(ret));
                break;
            }

            pktV->stream_index = oStreamV->index;
            av_packet_rescale_ts(pktV, codeCtxV->time_base, oStreamV->time_base);
            pktV->pos = -1;
            logPkt(ofmtctx,pktV);
            ret = av_interleaved_write_frame(ofmtctx, pktV);
            if (ret < 0) {
                LOGCATE("av_interleaved_write_frame failed:%s", av_err2str(ret));
                break;
            }
            pts_frame_indexV++;
            av_packet_unref(pktV);
        }
    }


}

int FFmpegEncodeAVToMp4::initEncodeAudio(){
    ret = avformat_alloc_output_context2(&ofmtctx, NULL, NULL, out_file_name);
    if (ret < 0) {
        LOGCATE("avformat_alloc_output_context2 failed:%s", av_err2str(ret));
        return ret;
    }
    AVOutputFormat *ofmt = ofmtctx->oformat;
    codecA = avcodec_find_encoder(ofmt->audio_codec);
    if (!codecA) {
        LOGCATE("can't find encoder");
        return ret;
    }
    LOGCATE("log coder:%d aac:%d", codecA->id, AV_CODEC_ID_AAC);
    codeCtxA = avcodec_alloc_context3(codecA);
    if (!codeCtxA) {
        LOGCATE("can't alloc codecContext");
        return ret;
    }
    configAudioEncodeParams(codeCtxA, codecA);
    av_dump_format(ofmtctx,0,out_file_name,1);
    ret = avcodec_open2(codeCtxA, codecA, NULL);
    LOGCATE("check open audio encoder resultStr:%s",av_err2str(ret));
    if (checkNegativeReturn(ret, "can't open encoder")) return ret;

    // create output ctx
    oStreamA = avformat_new_stream(ofmtctx, NULL);
    ret = avcodec_parameters_from_context(oStreamA->codecpar, codeCtxA);
    if (ret < 0){
        LOGCATE("avcodec_parameters_from_context failed:%s",av_err2str(ret));
        return ret;
    }
    oStreamA->codecpar->codec_tag = 0;


    frameA = av_frame_alloc();
    if (!frameA) {
        LOGCATE("can't alloc frame");
        return ret;
    }
    packetA = av_packet_alloc();
    if (!packetA) {
        LOGCATE("can't alloc packet");
        return ret;
    }
    configFrameParams(frameA, codeCtxA);
    int size = av_samples_get_buffer_size(NULL, codeCtxA->channels, codeCtxA->frame_size,
                                          codeCtxA->sample_fmt, 1);
    frame_bufferA = static_cast<uint8_t *>(av_malloc(size));
    avcodec_fill_audio_frame(frameA, codeCtxA->channels, codeCtxA->sample_fmt, (const uint8_t*)frame_bufferA, size, 1);
    if (checkNegativeReturn(ret, "can't alloc audio buffer")) return ret;

    //9.用于音频转码
    swr = swr_alloc();
    av_opt_set_channel_layout(swr, "in_channel_layout", AV_CH_LAYOUT_STEREO, 0);
    av_opt_set_channel_layout(swr, "out_channel_layout", AV_CH_LAYOUT_STEREO, 0);
    av_opt_set_int(swr, "in_sample_rate", 44100, 0);
    av_opt_set_int(swr, "out_sample_rate", 44100, 0);
    av_opt_set_sample_fmt(swr, "in_sample_fmt", AV_SAMPLE_FMT_S16, 0);
    av_opt_set_sample_fmt(swr, "out_sample_fmt", AV_SAMPLE_FMT_FLTP, 0);
    swr_init(swr);

    // encode loop


    LOGCATE("encode init finished");
    return ret;
}

int FFmpegEncodeAVToMp4::initEncodeVideo() {
    LOGCATE("begin init FFmpegEncodeVideo");

    AVOutputFormat *ofmt = ofmtctx->oformat;
    /* find the mpeg1video encoder */
    codecV = avcodec_find_encoder(ofmt->video_codec);
//    codecV = avcodec_find_encoder_by_name("h264_mediacodec");

    if (!codecV) {
        LOGCATE("Codec not found");
        exit(1);
    }

    codeCtxV = avcodec_alloc_context3(codecV);
    if (!codeCtxV) {
        LOGCATE("Could not allocate video codec context\n");
        exit(1);
    }

    codeCtxV->codec_id = codecV->id;
    codeCtxV->codec_type = AVMEDIA_TYPE_VIDEO;
    /* put sample parameters */
    codeCtxV->bit_rate = 3500 * 1000;
    /* resolution must be a multiple of two */
    codeCtxV->width = mWindow_height;
    codeCtxV->height = mWindow_width;
    /* frames per second */
    codeCtxV->time_base = (AVRational) {1, 25};
    codeCtxV->framerate = (AVRational) {25, 1};
    LOGCATE("log width:%d height:%d", codeCtxV->width, codeCtxV->height);
    codeCtxV->gop_size = 25;
    codeCtxV->max_b_frames = 1;
    codeCtxV->pix_fmt = AV_PIX_FMT_YUV420P;

    AVDictionary *opt = 0;
    if (codecV->id == AV_CODEC_ID_H264) {
        av_dict_set_int(&opt, "video_track_timescale", 25, 0);
        av_dict_set(&opt, "preset", "veryfast", 0);
        av_dict_set(&opt, "tune", "zerolatency", 0);
    }

    // create output ctx
    oStreamV = avformat_new_stream(ofmtctx, NULL);
    ret = avcodec_parameters_from_context(oStreamV->codecpar, codeCtxV);
    if (ret < 0) {
        LOGCATE("avcodec_parameters_from_context failed:%s", av_err2str(ret));
        return ret;
    }
    av_stream_set_r_frame_rate(oStreamV, {1, 25});
    oStreamV->codecpar->codec_tag = 0;
    if (!(ofmtctx->flags & AVFMT_NOFILE)){
        if((ret = avio_open(&ofmtctx->pb,out_file_name,AVIO_FLAG_WRITE)) < 0){
            LOGCATE("avio_open failed:%s",av_err2str(ret));
            return ret;
        }
    }

    /* open it */
    ret = avcodec_open2(codeCtxV, codecV, NULL);
    if (ret < 0) {
        LOGCATE("Could not open codec: %s\n", av_err2str(ret));
        exit(1);
    }


    pktV = av_packet_alloc();
    if (!pktV)
        exit(1);

    frameV = av_frame_alloc();
    if (!frameV) {
        LOGCATE("Could not allocate video frame\n");
        exit(1);
    }
    frameV->format = codeCtxV->pix_fmt;
    frameV->width = codeCtxV->width;
    frameV->height = codeCtxV->height;


    if (checkNegativeReturn(ret, "av_frame_get_buffer error :%s")) return ret;

    bufferSize = av_image_get_buffer_size(codeCtxV->pix_fmt, codeCtxV->width, codeCtxV->height, 1);
//    int lineSize = av_image_get_linesize(codeCtx->pix_fmt, codeCtx->width, 1);
//    pic_buf = static_cast<uint8_t *>(av_malloc(bufferSize));
    LOGCATE("print pic_size:%d fmt:%d width:%d height:%d", bufferSize, codeCtxV->pix_fmt,
            codeCtxV->width, codeCtxV->height);
//    ret = av_image_fill_arrays(frame->data,frame->linesize,pic_buf,codeCtx->pix_fmt,codeCtx->width,codeCtx->height,1);

    ret = av_frame_get_buffer(frameV, 1);
    if (ret < 0) {
        fprintf(stderr, "Could not allocate the video frame data\n");
        exit(1);
    }

    if (ret < 0) {
        LOGCATE("Could not allocate the video frame data\n");
        exit(1);
    }

    int y_size = codeCtxV->width * codeCtxV->height;
    LOGCATE("print compute_size:%d real_size:%d", bufferSize, y_size * 3 / 2);

    ret = avformat_write_header(ofmtctx, &opt);
    if (ret < 0) {
        LOGCATE("avformat_write_header failed :%s", av_err2str(ret));
        return ret;
    }

    return ret;
}


/**
 * 配置音频编码参数
 * @param pContext
 */
void FFmpegEncodeAVToMp4::configAudioEncodeParams(AVCodecContext *pContext, AVCodec *codec) {
    pContext->codec_id = codec->id;
    pContext->codec_type = AVMEDIA_TYPE_AUDIO;
    pContext->sample_fmt = AV_SAMPLE_FMT_FLTP;
    pContext->sample_rate = 44100;

    pContext->channel_layout = AV_CH_LAYOUT_STEREO;
    pContext->channels = av_get_channel_layout_nb_channels(pContext->channel_layout);
    pContext->bit_rate = 96000;
    pContext->profile = FF_PROFILE_AAC_MAIN;
    pContext->strict_std_compliance = FF_COMPLIANCE_EXPERIMENTAL;
//    AV_CODEC_ID_MP2
    // sample_fmt 二者选1
//    pContext->sample_fmt = AV_SAMPLE_FMT_S16; // 不支持

    if (!check_sample_fmt(codec, pContext->sample_fmt)) {
        LOGCATE("this sample format is not support");
    }
    if (ofmtctx->oformat->flags & AVFMT_GLOBALHEADER) {
        pContext->flags |= AV_CODEC_FLAG_GLOBAL_HEADER;
    }

    LOGCATE("look encode bit_rate:%jd sample_rate:%d  channel_layout:%jd channels:%d",
            pContext->bit_rate,
            pContext->sample_rate,
            pContext->channel_layout,
            pContext->channels);
}

int FFmpegEncodeAVToMp4::check_sample_fmt(const AVCodec *codec, enum AVSampleFormat sample_fmt)
{
    const enum AVSampleFormat *p = codec->sample_fmts;

    while (*p != AV_SAMPLE_FMT_NONE) {
        if (*p == sample_fmt)
            return 1;
        p++;
    }
    return 0;
}

void FFmpegEncodeAVToMp4::configFrameParams(AVFrame *pFrame, AVCodecContext *pContext) {
    pFrame->nb_samples = pContext->frame_size;
    pFrame->format = pContext->sample_fmt;

}


int FFmpegEncodeAVToMp4::select_sample_rate(AVCodec *codec) {
    const int *p;
    int best_samplerate = 0;

    if (!codec->supported_samplerates)
        return 44100;

    p = codec->supported_samplerates;
    while (*p) {
        if (!best_samplerate || abs(44100 - *p) < abs(44100 - best_samplerate))
            best_samplerate = *p;
        p++;
    }
    LOGCATE("pring best sample_rate:%d",best_samplerate);
    return best_samplerate;
}

uint64_t FFmpegEncodeAVToMp4::select_channel_layout(AVCodec *codec) {
    const uint64_t *p;
    uint64_t best_ch_layout = 0;
    int best_nb_channels = 0;

    if (!codec->channel_layouts)
        return AV_CH_LAYOUT_STEREO;

    p = codec->channel_layouts;
    while (*p) {
        int nb_channels = av_get_channel_layout_nb_channels(*p);

        if (nb_channels > best_nb_channels) {
            best_ch_layout = *p;
            best_nb_channels = nb_channels;
        }
        p++;
    }
    return best_ch_layout;
}


int FFmpegEncodeAVToMp4::check_sample_fmt(AVCodec *pCodec, AVSampleFormat sample_fmt) {
    const enum AVSampleFormat *p = pCodec->sample_fmts;

    while (*p != AV_SAMPLE_FMT_NONE) {
        if (*p == sample_fmt)
            return 1;
        p++;
    }
    return 0;
}

void FFmpegEncodeAVToMp4::logPkt(AVFormatContext *pContext, AVPacket *pkt) {
    AVRational *time_base = &ofmtctx->streams[pkt->stream_index]->time_base;

    LOGCATE("some detail info ----- pts:%s pts_time:%s dts:%s dts_time:%s duration:%s duration_time:%s stream_index:%d\n",
           av_ts2str(pkt->pts), av_ts2timestr(pkt->pts, time_base),
           av_ts2str(pkt->dts), av_ts2timestr(pkt->dts, time_base),
           av_ts2str(pkt->duration), av_ts2timestr(pkt->duration, time_base),
           pkt->stream_index);
}

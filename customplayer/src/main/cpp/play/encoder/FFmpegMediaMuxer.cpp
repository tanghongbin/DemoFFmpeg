//
// Created by Admin on 2021/5/31.
//



#include <encoder/FFmpegMediaMuxer.h>
#include <utils/utils.h>
#include <utils/AudioRecordPlayHelper.h>
#include <encoder/FFmpegEncodeAudio.h>
#include <utils/CustomGLUtils.h>
#include <render/VideoRender.h>
#include <libyuv.h>
#include <libyuv/scale.h>
#include <libyuv/convert.h>
#include <encoder/FFmpegEncodeVideo.h>
#include <libyuv/rotate.h>
#include <render/VideoFboRender.h>

extern "C" {
#include <libswscale/swscale.h>
#include <libavutil/avassert.h>
#include <libavformat/avformat.h>
#include <libavutil/opt.h>
#include <libswresample/swresample.h>
#include <libavutil/timestamp.h>
#include <libavutil/time.h>
}

//#define STREAM_DURATION   10.0
#define STREAM_FRAME_RATE 25 /* 25 images/s */
#define STREAM_PIX_FMT    AV_PIX_FMT_YUV420P /* default pix_fmt */


#define SCALE_FLAGS SWS_BICUBIC

FFmpegMediaMuxer* FFmpegMediaMuxer::instance = nullptr;


 void log_packet(const AVFormatContext *fmt_ctx, const AVPacket *pkt)
{
    AVRational *time_base = &fmt_ctx->streams[pkt->stream_index]->time_base;

     LOGCATE("pts:%s pts_time:%s dts:%s dts_time:%s duration:%s duration_time:%s stream_index:%d\n",
            av_ts2str(pkt->pts), av_ts2timestr(pkt->pts, time_base),
            av_ts2str(pkt->dts), av_ts2timestr(pkt->dts, time_base),
            av_ts2str(pkt->duration), av_ts2timestr(pkt->duration, time_base),
            pkt->stream_index);
}

 int write_frame(AVFormatContext *fmt_ctx, AVCodecContext *c,
                 AVStream *st, AVFrame *frame)
{
    int ret;

    // send the frame to the encoder
    ret = avcodec_send_frame(c, frame);
    if (ret < 0) {
        fprintf(stderr, "Error sending a frame to the encoder: %s\n",
                av_err2str(ret));
        exit(1);
    }

    while (ret >= 0) {
        AVPacket pkt = { 0 };

        ret = avcodec_receive_packet(c, &pkt);
        if (ret == AVERROR(EAGAIN) || ret == AVERROR_EOF){
            break;
        }else if (ret < 0) {
            fprintf(stderr, "Error encoding a frame: %s\n", av_err2str(ret));
            exit(1);
        }

        /* rescale output packet timestamp values from codec to stream timebase */
        av_packet_rescale_ts(&pkt, c->time_base, st->time_base);
        pkt.stream_index = st->index;

        /* Write the compressed frame to the media file. */
        log_packet(fmt_ctx, &pkt);
        ret = av_interleaved_write_frame(fmt_ctx, &pkt);
        av_packet_unref(&pkt);
        if (ret < 0) {
            fprintf(stderr, "Error while writing output packet: %s\n", av_err2str(ret));
            exit(1);
        }
    }

//    LOGCATE("has write result:%d  msg:%s currentIndex:%d currentCodeId:%d",ret,av_err2str(ret),st->index,c->codec_id);

    return 1;
}

/* Add an output stream. */
 void add_stream(OutputStream *ost, AVFormatContext *oc,
                       AVCodec **codec,
                       enum AVCodecID codec_id)
{
    AVCodecContext *c;
    int i;

    /* find the encoder */
    *codec = avcodec_find_encoder(codec_id);
    if (!(*codec)) {
        fprintf(stderr, "Could not find encoder for '%s'\n",
                avcodec_get_name(codec_id));
        exit(1);
    }

    ost->st = avformat_new_stream(oc, NULL);
    if (!ost->st) {
        fprintf(stderr, "Could not allocate stream\n");
        exit(1);
    }
    ost->st->id = oc->nb_streams-1;
    c = avcodec_alloc_context3(*codec);
    if (!c) {
        fprintf(stderr, "Could not alloc an encoding context\n");
        exit(1);
    }
    ost->enc = c;

    switch ((*codec)->type) {
        case AVMEDIA_TYPE_AUDIO:
            c->sample_fmt  = (*codec)->sample_fmts ?
                             (*codec)->sample_fmts[0] : AV_SAMPLE_FMT_FLTP;
            c->bit_rate    = 64000;
            c->sample_rate = 44100;
            if ((*codec)->supported_samplerates) {
                c->sample_rate = (*codec)->supported_samplerates[0];
                for (i = 0; (*codec)->supported_samplerates[i]; i++) {
                    if ((*codec)->supported_samplerates[i] == 44100)
                        c->sample_rate = 44100;
                }
            }
            c->channels        = av_get_channel_layout_nb_channels(c->channel_layout);
            c->channel_layout = AV_CH_LAYOUT_STEREO;
            if ((*codec)->channel_layouts) {
                c->channel_layout = (*codec)->channel_layouts[0];
                for (i = 0; (*codec)->channel_layouts[i]; i++) {
                    if ((*codec)->channel_layouts[i] == AV_CH_LAYOUT_STEREO)
                        c->channel_layout = AV_CH_LAYOUT_STEREO;
                }
            }
            c->channels        = av_get_channel_layout_nb_channels(c->channel_layout);
            ost->st->time_base = (AVRational){ 1, c->sample_rate };
            break;

        case AVMEDIA_TYPE_VIDEO:
            c->codec_id = codec_id;

            c->bit_rate = 3500 * 1000;
            /* Resolution must be a multiple of two. */
            c->width    = 720;
            c->height   = 1280;
            /* timebase: This is the fundamental unit of time (in seconds) in terms
             * of which frame timestamps are represented. For fixed-fps content,
             * timebase should be 1/framerate and timestamp increments should be
             * identical to 1. */
            ost->st->time_base = (AVRational){ 1, STREAM_FRAME_RATE };
            c->time_base       = ost->st->time_base;
            c->gop_size      = 1; /* emit one intra frame every twelve frames at most */
            c->pix_fmt       = STREAM_PIX_FMT;
            if (c->codec_id == AV_CODEC_ID_MPEG2VIDEO) {
                /* just for testing, we also add B-frames */
                c->max_b_frames = 2;
            }
            if (c->codec_id == AV_CODEC_ID_MPEG1VIDEO) {
                /* Needed to avoid using macroblocks in which some coeffs overflow.
                 * This does not happen with normal video, it just happens here as
                 * the motion of the chroma plane does not match the luma plane. */
                c->mb_decision = 2;
            }
            int videoFrameSize = c->width * c->height * 3 / 2;
            FFmpegMediaMuxer::getInstace() -> videoFrameDst = static_cast<uint8_t *>(av_malloc(videoFrameSize));
            FFmpegMediaMuxer::getInstace()->cameraWidth = c->height;
            FFmpegMediaMuxer::getInstace()->cameraHeight = c->width;
            break;
    }

    /* Some formats want stream headers to be separate. */
    if (oc->oformat->flags & AVFMT_GLOBALHEADER)
        c->flags |= AV_CODEC_FLAG_GLOBAL_HEADER;
}

/**************************************************************/
/* audio output */

 AVFrame *alloc_audio_frame(enum AVSampleFormat sample_fmt,
                                  uint64_t channel_layout,
                                  int sample_rate, int nb_samples)
{
    AVFrame *frame = av_frame_alloc();
    int ret;

    if (!frame) {
        fprintf(stderr, "Error allocating an audio frame\n");
        exit(1);
    }

    frame->format = sample_fmt;
    frame->channel_layout = channel_layout;
    frame->sample_rate = sample_rate;
    frame->nb_samples = nb_samples;

    if (nb_samples) {
        ret = av_frame_get_buffer(frame, 0);
        if (ret < 0) {
            fprintf(stderr, "Error allocating an audio buffer\n");
            exit(1);
        }
    }

    return frame;
}

 void open_audio(AVFormatContext *oc, AVCodec *codec, OutputStream *ost, AVDictionary *opt_arg)
{
    AVCodecContext *c;
    int nb_samples;
    int ret;
    AVDictionary *opt = NULL;

    c = ost->enc;

    /* open it */
    av_dict_copy(&opt, opt_arg, 0);
    ret = avcodec_open2(c, codec, &opt);
    av_dict_free(&opt);
    if (ret < 0) {
        fprintf(stderr, "Could not open audio codec: %s\n", av_err2str(ret));
        exit(1);
    }

    /* init signal generator */
    ost->t     = 0;
    ost->tincr = 2 * M_PI * 110.0 / c->sample_rate;
    /* increment frequency by 110 Hz per second */
    ost->tincr2 = 2 * M_PI * 110.0 / c->sample_rate / c->sample_rate;

    if (c->codec->capabilities & AV_CODEC_CAP_VARIABLE_FRAME_SIZE)
        nb_samples = 10000;
    else
        nb_samples = c->frame_size;

    ost->frame     = alloc_audio_frame(c->sample_fmt, c->channel_layout,
                                       c->sample_rate, nb_samples);
    ost->tmp_frame = alloc_audio_frame(AV_SAMPLE_FMT_S16, c->channel_layout,
                                       c->sample_rate, nb_samples);

    /* copy the stream parameters to the muxer */
    ret = avcodec_parameters_from_context(ost->st->codecpar, c);
    if (ret < 0) {
        fprintf(stderr, "Could not copy the stream parameters\n");
        exit(1);
    }

    /* create resampler context */
    ost->swr_ctx = swr_alloc();
    if (!ost->swr_ctx) {
        fprintf(stderr, "Could not allocate resampler context\n");
        exit(1);
    }

    /* set options */
    av_opt_set_int       (ost->swr_ctx, "in_channel_count",   c->channels,       0);
    av_opt_set_int       (ost->swr_ctx, "in_sample_rate",     c->sample_rate,    0);
    av_opt_set_sample_fmt(ost->swr_ctx, "in_sample_fmt",      AV_SAMPLE_FMT_S16, 0);
    av_opt_set_int       (ost->swr_ctx, "out_channel_count",  c->channels,       0);
    av_opt_set_int       (ost->swr_ctx, "out_sample_rate",    c->sample_rate,    0);
    av_opt_set_sample_fmt(ost->swr_ctx, "out_sample_fmt",     c->sample_fmt,     0);

    /* initialize the resampling context */
    if ((ret = swr_init(ost->swr_ctx)) < 0) {
        fprintf(stderr, "Failed to initialize the resampling context\n");
        exit(1);
    }
}

#define IS_ENABLE_TEST false

/* Prepare a 16 bit dummy audio frame of 'frame_size' samples and
 * 'nb_channels' channels. */
 AVFrame *get_audio_frame(OutputStream *ost)
{
#if IS_ENABLE_TEST
    AVFrame *frame = ost->tmp_frame;
    int j, i, v;
    int16_t *q = (int16_t*)frame->data[0];
    int16_t *q2 = (int16_t*)frame->data[1];

    /* check if we want to generate more frames */
    if (av_compare_ts(ost->next_pts, ost->enc->time_base,
                      STREAM_DURATION, (AVRational){ 1, 1 }) >= 0)
        return NULL;

    for (j = 0; j <frame->nb_samples; j++) {
        v = (int)(sin(ost->t) * 10000);
//        for (i = 0; i < ost->enc->channels; i++)
            *q++ = v;
            *q2++ = v;
        ost->t     += ost->tincr;
        ost->tincr += ost->tincr2;
    }

    frame->pts = ost->next_pts;
    ost->next_pts  += frame->nb_samples;

    return frame;
#else
    AVFrame *frame = ost->tmp_frame;

    /* check if we want to generate more frames */
//    if (av_compare_ts(ost->next_pts, ost->enc->time_base,
//                      STREAM_DURATION, (AVRational){ 1, 1 }) >= 0)
//        return NULL;
    frame->pts = ost->next_pts;
    ost->next_pts  += frame->nb_samples;
    return frame;
#endif
}

/*
 * encode one audio frame and send it to the muxer
 * return 1 when encoding is finished, 0 otherwise
 */
 int write_audio_frame(AVFormatContext *oc, OutputStream *ost)
{
    AVCodecContext *c;
    AVPacket pkt = { 0 }; // data and size must be 0;
    AVFrame *frame;
    int ret;
    int got_packet;
    int dst_nb_samples;

    AudioRecordItemInfo *audioInfo = FFmpegMediaMuxer::getInstace()->audioQueue.popFirst();
    while (!audioInfo){
        av_usleep(10 * 1000);
    }
    if (!audioInfo){
//        LOGCATE("audio info is empty");
        return 1;
    }

    av_init_packet(&pkt);
    c = ost->enc;

    frame = get_audio_frame(ost);
    if (!frame) {
//        LOGCATE("frame is null");
        return 1;
    }

//    LOGCATE("log audioInfo:%p frame:%p data:%p samples:%d",audioInfo,frame,audioInfo->data,audioInfo->nb_samples);
    frame->data[0] = audioInfo->data;
    frame->pts = ost->next_pts;
    ost->next_pts +=  frame->nb_samples;

    LOGCATE("log frame nb_samples:%d audioInfosamples:%d",frame->nb_samples,audioInfo->nb_samples);

    if (frame) {
        /* convert samples from native format to destination codec format, using the resampler */
        /* compute destination number of samples */
        dst_nb_samples = av_rescale_rnd(swr_get_delay(ost->swr_ctx, c->sample_rate) + frame->nb_samples,
                                        c->sample_rate, c->sample_rate, AV_ROUND_UP);
//        av_assert0(dst_nb_samples == frame->nb_samples);

        /* when we pass a frame to the encoder, it may keep a reference to it
         * internally;
         * make sure we do not overwrite it here
         */
        ret = av_frame_make_writable(ost->frame);
        if (ret < 0)
            exit(1);
//        LOGCATE("log swr convert outSample:%d inSample:%d",frame->nb_samples,audioInfo->nb_samples);
        /* convert to destination format */
        ret = swr_convert(ost->swr_ctx,
                          ost->frame->data, ost->frame->nb_samples,
                          (const uint8_t **)frame->data, audioInfo->nb_samples/4);
        if (ret < 0) {
            fprintf(stderr, "Error while converting\n");
            exit(1);
        }

        frame = ost->frame;

        frame->pts = av_rescale_q(ost->samples_count, (AVRational){1, c->sample_rate}, c->time_base);
        LOGCATE("log current pts:%lld sampleCount:%d  sampleRate:%d c-timeBase:%d --%d" ,frame->pts,ost->samples_count,
                c->sample_rate,c->time_base.num,c->time_base.den);
        ost->samples_count += frame->nb_samples;
    }

    ret = write_frame(oc, c, ost->st, frame);

    if (audioInfo){
        audioInfo->recycle();
    }
    audioInfo = nullptr;
    return 1;
}

/**************************************************************/
/* video output */

 AVFrame *alloc_picture(enum AVPixelFormat pix_fmt, int width, int height)
{
    AVFrame *picture;
    int ret;

    picture = av_frame_alloc();
    if (!picture)
        return NULL;

    picture->format = pix_fmt;
    picture->width  = width;
    picture->height = height;

    /* allocate the buffers for the frame data */
    ret = av_frame_get_buffer(picture, 1);
    if (ret < 0) {
        fprintf(stderr, "Could not allocate frame data.\n");
        exit(1);
    }

    return picture;
}

 void open_video(AVFormatContext *oc, AVCodec *codec, OutputStream *ost, AVDictionary *opt_arg)
{
    int ret;
    AVCodecContext *c = ost->enc;
    AVDictionary *opt = NULL;

    av_dict_copy(&opt, opt_arg, 0);

    /* open the codec */
    ret = avcodec_open2(c, codec, &opt);
    av_dict_free(&opt);
    if (ret < 0) {
        fprintf(stderr, "Could not open video codec: %s\n", av_err2str(ret));
        exit(1);
    }

    /* allocate and init a re-usable frame */
    ost->frame = alloc_picture(c->pix_fmt, c->width, c->height);
    if (!ost->frame) {
        fprintf(stderr, "Could not allocate video frame\n");
        exit(1);
    }


    /* If the output format is not YUV420P, then a temporary YUV420P
     * picture is needed too. It is then converted to the required
     * output format. */
    int tmpWidth = c->width,tmpHeight = c->height;
    ost->tmp_frame = alloc_picture(AV_PIX_FMT_RGBA, tmpWidth, tmpHeight);
    if (!ost->tmp_frame) {
        fprintf(stderr, "Could not allocate temporary picture\n");
        exit(1);
    }

    /* copy the stream parameters to the muxer */
    ret = avcodec_parameters_from_context(ost->st->codecpar, c);
    if (ret < 0) {
        fprintf(stderr, "Could not copy the stream parameters\n");
        exit(1);
    }
}


AVFrame *get_video_frame(OutputStream *ost, NativeOpenGLImage *openGlImage) {
    AVCodecContext *c = ost->enc;
    int ret;

    /* check if we want to generate more frames */
//    if (av_compare_ts(ost->next_pts, c->time_base,
//                      STREAM_DURATION, (AVRational){ 1, 1 }) > 0)
//        return NULL;

    /* when we pass a frame to the encoder, it may keep a reference to it
     * internally; make sure we do not overwrite it here */
    if (av_frame_make_writable(ost->frame) < 0)
        exit(1);

bool isSwcConvert = false;
    AVPixelFormat srcFormat;
if (isSwcConvert) srcFormat = AV_PIX_FMT_NV21;

else srcFormat = AV_PIX_FMT_RGBA;


//    LOGCATE("打印目标格式：%d",c->pix_fmt == AV_PIX_FMT_YUV420P);
    if (srcFormat != AV_PIX_FMT_YUV420P) {
        /* as we only generate a YUV420P picture, we must convert it
         * to the codec pixel format if needed */
        if (!ost->sws_ctx) {
            ost->sws_ctx = sws_getContext(c->width, c->height,
                                          srcFormat,
                                          c->width, c->height,
                                          c->pix_fmt,
                                          SCALE_FLAGS, NULL, NULL, NULL);
            if (!ost->sws_ctx) {
                fprintf(stderr,
                        "Could not initialize the conversion context\n");
                exit(1);
            }
        }
        ost->tmp_frame->data[0] = openGlImage->ppPlane[0];
        ret = sws_scale(ost->sws_ctx, (const uint8_t * const *) ost->tmp_frame->data,
                  ost->tmp_frame->linesize, 0, c->height, ost->frame->data,
                  ost->frame->linesize);
        LOGCATE("log convert result:%d lineSize:%d dstLineSize:%d",ret,ost->tmp_frame->linesize[0],
                ost->frame->linesize[0]);

        // 感觉这里转换有问题,用libyuv 试一下
    } else {
        // 这里转换一下
        uint8_t * dstData = new uint8_t [ost->frame->width * ost->frame->height * 3/2];
        yuvRgbaToI420(openGlImage->ppPlane[0],dstData,ost->frame->width,ost->frame->height);
        ost->frame->data[0] = dstData;
        ost->frame->data[1] = dstData + ost->frame->width * ost->frame->height;
        ost->frame->data[2] = dstData + ost->frame->width * ost->frame->height * 5/4;
        LOGCATE("打印转换 w:%d h:%d",ost->frame->width,ost->frame->height);
//        fill_yuv_image(, ost->next_pts, rgbaData);
    }

    ost->frame->pts = ost->next_pts++;

//    LOGCATE("video frame has prepared");

    return ost->frame;
}




/*
 * encode one video frame and send it to the muxer
 * return 1 when encoding is finished, 0 otherwise
 */
 int write_video_frame(AVFormatContext *oc, OutputStream *ost)
{
//     LOGCATE("start encode video");
    NativeOpenGLImage *nativeOpenGlImage = FFmpegMediaMuxer::getInstace()->videoQueue.popFirst();
    if (!nativeOpenGlImage){
        return 1;
    }
    AVFrame *frameData = get_video_frame(ost, nativeOpenGlImage);
    if (!frameData) return 1;
    int ret = write_frame(oc, ost->enc, ost->st, frameData);
    NativeOpenGLImageUtil::FreeNativeImage(nativeOpenGlImage);
    delete nativeOpenGlImage;
    return ret;
}

static void close_stream(AVFormatContext *oc, OutputStream *ost)
{
    avcodec_free_context(&ost->enc);
    av_frame_free(&ost->frame);
    av_frame_free(&ost->tmp_frame);
    sws_freeContext(ost->sws_ctx);
    swr_free(&ost->swr_ctx);
}

/**************************************************************/
/* media file output */



int FFmpegMediaMuxer::StartMuxer(const char * outFileName)
{
    av_usleep(1500 * 1000);
    LOGCATE("start muxer");
    OutputStream video_st = { 0 }, audio_st = { 0 };
    const char *filename;
    AVOutputFormat *fmt;
    AVFormatContext *oc;
    AVCodec *audio_codec, *video_codec;
    int ret;
    int have_video = 0, have_audio = 0;
    int encode_video = 0, encode_audio = 0;
    AVDictionary *opt = NULL;
    int i;

    filename = outFileName;

    /* allocate the output media context */
    avformat_alloc_output_context2(&oc, NULL, NULL, filename);
    if (!oc) {
        printf("Could not deduce output format from file extension: using MPEG.\n");
        avformat_alloc_output_context2(&oc, NULL, "mpeg", filename);
    }
    if (!oc)
        return 1;

    fmt = oc->oformat;

    /* Add the audio and video streams using the default format codecs
     * and initialize the codecs. */
    if (fmt->video_codec != AV_CODEC_ID_NONE) {
        add_stream(&video_st, oc, &video_codec, fmt->video_codec);
        have_video = 1;
        encode_video = 1;
    }
    if (fmt->audio_codec != AV_CODEC_ID_NONE) {
        add_stream(&audio_st, oc, &audio_codec, fmt->audio_codec);
        have_audio = 1;
        encode_audio = 1;
    }

    /* Now that all the parameters are set, we can open the audio and
     * video codecs and allocate the necessary encode buffers. */
    if (have_video)
        open_video(oc, video_codec, &video_st, opt);

    if (have_audio)
        open_audio(oc, audio_codec, &audio_st, opt);

    av_dump_format(oc, 0, filename, 1);

    /* open the output file, if needed */
    if (!(fmt->flags & AVFMT_NOFILE)) {
        ret = avio_open(&oc->pb, filename, AVIO_FLAG_WRITE);
        if (ret < 0) {
            fprintf(stderr, "Could not open '%s': %s\n", filename,
                    av_err2str(ret));
            return 1;
        }
    }

    /* Write the stream header, if any. */
    ret = avformat_write_header(oc, &opt);
    if (ret < 0) {
        fprintf(stderr, "Error occurred when opening output file: %s\n",
                av_err2str(ret));
        return 1;
    }

    encodeMediaAV(oc, encode_video, encode_audio, video_st, audio_st);

    /* Write the trailer, if any. The trailer must be written before you
     * close the CodecContexts open when you wrote the header; otherwise
     * av_write_trailer() may try to use memory that was freed on
     * av_codec_close(). */
    av_write_trailer(oc);

    AudioRecordPlayHelper::getInstance()->stopCapture();
    /* Close each codec. */
    if (have_video)
        close_stream(oc, &video_st);
    if (have_audio)
        close_stream(oc, &audio_st);

    if (!(fmt->flags & AVFMT_NOFILE))
        /* Close the output file. */
        avio_closep(&oc->pb);

    /* free the stream */
    avformat_free_context(oc);

    return 0;
}

void FFmpegMediaMuxer::encodeMediaAV(AVFormatContext *oc, int video_editable, int audio_editable,
                                     OutputStream &video_st, OutputStream &audio_st) {
    int size = av_samples_get_buffer_size(NULL, audio_st.frame->channels, audio_st.frame->nb_samples,
                                          audio_st.enc->sample_fmt, 1);
    LOGCATE("打印音频缓冲大小:%d",size);
    int64_t start = GetSysCurrentTime();
#define TESTMODE 3 // 1-音频，2-视频，3混合
    while (!FFmpegMediaMuxer::getInstace() -> isDestroyed && (video_editable || audio_editable)) {
        /* select the stream to encode */
//        LOGCATE("differ :%lld",GetSysCurrentTime() - start);
//        if (GetSysCurrentTime() - start > 10 * 1000) break;
#if TESTMODE == 1
        encode_audio = !write_audio_frame(oc, &audio_st);
#elif TESTMODE == 2
        encode_video = !write_video_frame(oc, &video_st);
#else
        int compareResult = av_compare_ts(video_st.next_pts, video_st.enc->time_base,
                                          audio_st.next_pts, audio_st.enc->time_base);
        int64_t aTime = audio_st.next_pts * av_q2d(oc->streams[audio_st.st->index]->time_base);
        int64_t vTime = video_st.next_pts * av_q2d(oc->streams[video_st.st->index]->time_base);
        LOGCATE("log encode video:%d encodeaudio:%d compareresult:%d  av next pts:%lld/%lld   a/v time: %lld/%lld",
                video_editable,audio_editable,compareResult,audio_st.next_pts,video_st.next_pts,aTime,vTime);
        if (video_editable &&
            (!audio_editable || compareResult <= 0)) {
//            LOGCATE("log start encode_video");
            video_editable = write_video_frame(oc, &video_st);
        } else {
//            LOGCATE("log start  encode_audio");
            audio_editable = write_audio_frame(oc, &audio_st);
        }
//if (encode_audio){
//    encode_audio = !write_audio_frame(oc, &audio_st);
//    LOGCATE("log start  encode_audio");
//} else {
//    write_video_frame(oc, &video_st);
//    LOGCATE("log start encode_video");
//}
#endif

    }
    LOGCATE("loop audio encode has end:%lld",GetSysCurrentTime() - start);
}


int FFmpegMediaMuxer::init(const char * fileName){
    strcpy(mTargetFilePath,fileName);
    LOGCATE("打印地址:%s",fileName);
    access(fileName,0);
    audioRecordThread = new std::thread(startRecord,this);
    thread = new std::thread(StartMuxer,mTargetFilePath);
    return 0;
}


void FFmpegMediaMuxer::receivePixelData(int type,NativeOpenGLImage *pVoid){
    FFmpegMediaMuxer::getInstace()->OnCameraFrameDataValible(type, pVoid);
}

void FFmpegMediaMuxer::receiveAudioBuffer(uint8_t* data,int nb_samples){
//    LOGCATE("打印receiveAudioBuffer data的大小：%d",BUFFER_SIZE);
    auto* info = new AudioRecordItemInfo;
    auto * newResult = static_cast<uint8_t *>(malloc(BUFFER_SIZE));
    memcpy(newResult,data,BUFFER_SIZE);
    info->nb_samples = nb_samples;
    info->data = newResult;
    getInstace() -> audioQueue.pushLast(info);
//    LOGCATE("打印数据总数：%d ",getInstace()->audioQueue.size());
}

void FFmpegMediaMuxer::startRecord(void * pVoid){
    AudioRecordPlayHelper::getInstance()->startCapture(FFmpegMediaMuxer::receiveAudioBuffer);
}

static int64_t currentDuration = 0;

static void testAudio(uint8_t* data,int samples){
    FFmpegEncodeAudio::getInstance()->encodeAudioFrame(data,samples);
    if (GetSysCurrentTime() - currentDuration > 10 * 1000) {
        AudioRecordPlayHelper::getInstance()->stopCapture();
    }
}

void FFmpegMediaMuxer::test(int type){
    if (type == 1) {
        currentDuration = GetSysCurrentTime();
        FFmpegEncodeAudio* encodeAudio = FFmpegEncodeAudio::getInstance();
        encodeAudio->init("test.aac");
        AudioRecordPlayHelper::getInstance()->startCapture(testAudio);
        encodeAudio->unInit();
        encodeAudio->destroyInstance();
    } else if(type == 3) {
        currentDuration = GetSysCurrentTime();
        FFmpegEncodeVideo* encodeVideo = FFmpegEncodeVideo::getInstance();
        encodeVideo->mWindow_width = 1280;
        encodeVideo->mWindow_height = 720;
        encodeVideo->init();
    }

}



void FFmpegMediaMuxer::OnSurfaceCreate() {
    videoRender = new VideoFboRender;
    videoRender->readPixelCall = FFmpegMediaMuxer::receivePixelData;
    videoRender->Init();
}
void FFmpegMediaMuxer::OnSurfaceChanged(int width,int height) {
    videoRender->OnSurfaceChanged(width,height);
}
void FFmpegMediaMuxer::OnCameraFrameDataValible(int type,NativeOpenGLImage * srcData) {
    // 4-glreadPixel 读出来的数据已经转换成720分辨率的420p的数据了,可以直接编码
    if (type == 1) {
        // 1-java camera nv21 转换成i420直接编码成mp4  - 适用于surfaceview
        if (isDestroyed || cameraWidth == 0 || cameraHeight == 0) return;
//        int bufferSize = cameraWidth * cameraHeight * 3 / 2;
//        uint8_t * i420Dst = new uint8_t [bufferSize];
//        memset(i420Dst,0x00,bufferSize);
//        yuvNv21To420p(srcData,i420Dst,cameraWidth,cameraHeight,libyuv::kRotate90);
//        uint8_t * lastData = videoQueue.pushLast(i420Dst);
//        if (lastData) delete [] lastData;
    } else if (type == 2) {
        // camera-yuv420p 数据，直接渲染成Opengles
        int localWidth = 1280;
        int localHeight = 720;
        uint8_t * i420srcData = srcData->ppPlane[0];
        uint8_t * i420dstData = new uint8_t [localWidth * localHeight * 3/2];
        yuvI420Rotate(i420srcData, i420dstData, localWidth, localHeight, libyuv::kRotate270);
        srcData -> width = localHeight;
        srcData -> height = localWidth;
        srcData -> format = IMAGE_FORMAT_I420;
        srcData -> ppPlane[0] = i420dstData;
        srcData -> ppPlane[1] = i420dstData + localWidth * localHeight;
        srcData -> ppPlane[2] = i420dstData + localWidth * localHeight * 5 / 4;
        srcData -> pLineSize[0] = localHeight;
        srcData -> pLineSize[1] = localHeight >> 1;
        srcData -> pLineSize[2] = localHeight >> 1;
        videoRender->copyImage(srcData);
        delete [] i420dstData;
//        LOGCATE("log width:%d height:%d imagewidth:%d imageheight:%d",localWidth,localHeight,openGlImage.width,openGlImage.height);
    } else if (type == 3) {
        // 3-rgba放进队列
        NativeOpenGLImage * lastData = videoQueue.pushLast(srcData);
        if (lastData) {
            NativeOpenGLImageUtil::FreeNativeImage(lastData);
            delete lastData;
        }
    } else if (type == 4) {
        // glreadypixel 后转换成720分辨率的420p的画面,可以直接使用
        NativeOpenGLImage * lastData = videoQueue.pushLast(srcData);
        if (lastData) {
            NativeOpenGLImageUtil::FreeNativeImage(lastData);
            delete lastData;
        }
    } else {
        LOGCATE("OnCameraFrameDataValible not support type:%d",type);
    }


}

void FFmpegMediaMuxer::OnDrawFrame(){
    if (videoRender) videoRender->DrawFrame();
}

void FFmpegMediaMuxer::Destroy(){
    // 清空所有缓存音频
    isDestroyed = true;
    FFmpegEncodeVideo::getInstance()->unInit();
    LOGCATE("start destroy :%d",audioQueue.size());
    AudioRecordPlayHelper::getInstance()->stopCapture();
    if (thread) {
        thread -> join();
        delete thread;
    }
    if(videoRender){
        videoRender->Destroy();
    }
    if (audioRecordThread){
        audioRecordThread->join();
        delete audioRecordThread;
    }
    if (audioQueue.size() != 0){
        for (int i = 0; i < audioQueue.size(); ++i) {
            AudioRecordItemInfo *itemInfo = audioQueue.popFirst();
            itemInfo->recycle();
        }
    }
    if (videoQueue.size() != 0){
        for (int i = 0; i < videoQueue.size(); ++i) {
            NativeOpenGLImage * data= videoQueue.popFirst();
            NativeOpenGLImageUtil::FreeNativeImage(data);
            delete data;
        }
    }
    if (videoFrameDst){
        av_free(videoFrameDst);
    }
    AudioRecordPlayHelper::getInstance()->destroyInstance();
    delete instance;
    instance = 0;
}
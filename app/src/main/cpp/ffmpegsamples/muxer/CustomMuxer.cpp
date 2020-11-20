//
// Created by Admin on 2020/11/19.
//

#include <utils.h>

#include <CustomGLUtils.h>
#include "CustomMuxer.h"
#include "BitStreamFilterHelper.h"



void CustomMuxer::muxer() {

    const char *inputAudioUrl = "/storage/emulated/0/ffmpegtest/splitFiles/aac_encoded.aac";
    const char *inputVideoUrl = "/storage/emulated/0/ffmpegtest/splitFiles/h264_encoded.h264";
    const char *outputFileUrl = getRandomStr("mp4_", ".mp4", "splitFiles/");

    LOGCATE("has enter muxer:%s \n video:%s", inputAudioUrl, inputVideoUrl);
    int ret;

    AVFormatContext *imft_audio, *imft_video, *ofmt, *tempifmt;
    int audioIn_index = -1, videoIn_index = -1, audioOut_index = -1, videoOut_index = -1;
    AVPacket *pkt;
    int64_t cur_audio_pts = 0, cur_video_pts = 0;
    int frame_index = 0;

    // alloc audio ctx
    imft_audio = avformat_alloc_context();
    if (!imft_audio) {
        ret = -1;
        goto End;
    }
    ret = avformat_open_input(&imft_audio, inputAudioUrl, NULL, NULL);
    if (ret < 0) {
        LOGCATE("open audio input failed:%s", av_err2str(ret));
        goto End;
    }
    ret = avformat_find_stream_info(imft_audio, NULL);
    if (ret < 0) {
        LOGCATE("can't find audio stream info :%s", av_err2str(ret));
        goto End;
    }
    for (int i = 0; i < imft_audio->nb_streams; ++i) {
        if (imft_audio->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_AUDIO) {
            audioIn_index = i;
        }
    }
    LOGCATE("find audio stream index:%d", audioIn_index);


    // alloc video ctx
    imft_video = avformat_alloc_context();
    if (!imft_video) {
        ret = -1;
        goto End;
    }
    ret = avformat_open_input(&imft_video, inputVideoUrl, NULL, NULL);
    if (ret < 0) {
        LOGCATE("open video input failed:%s", av_err2str(ret));
        goto End;
    }
    ret = avformat_find_stream_info(imft_video, NULL);
    if (ret < 0) {
        LOGCATE("can't find video stream info :%s", av_err2str(ret));
        goto End;
    }
    for (int i = 0; i < imft_video->nb_streams; ++i) {
        if (imft_video->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_VIDEO) {
            videoIn_index = i;
        }
    }
    LOGCATE("find video stream index:%d", videoIn_index);


    // alloc output ctx
    ret = avformat_alloc_output_context2(&ofmt, NULL, NULL, outputFileUrl);
    if (ret < 0) {
        LOGCATE("avformat_alloc_output_context2 failed :%s", av_err2str(ret));
        goto End;
    }
    // add stream
    if (audioIn_index == -1 || videoIn_index == -1) {
        LOGCATE("stream_index can't fetch from stream");
        goto End;
    }
    ret = addNewStream(imft_audio, ofmt, audioIn_index, &audioOut_index);
    if (ret < 0) {
        LOGCATE("addNewStream audio failed :%s", av_err2str(ret));
        goto End;
    }
    ret = addNewStream(imft_video, ofmt, videoIn_index, &videoOut_index);
    if (ret < 0) {
        LOGCATE("addNewStream video failed :%s", av_err2str(ret));
        goto End;
    }
    LOGCATE("log audioout_index:%d videoout_index:%d", audioOut_index, videoOut_index);

    if (!(ofmt->flags & AVFMT_NOFILE)) {
        if ((ret = avio_open(&ofmt->pb, outputFileUrl, AVIO_FLAG_WRITE)) < 0) {
            LOGCATE("avio_open failed :%s", av_err2str(ret));
            goto End;
        }
    }
    pkt = av_packet_alloc();
    if (!pkt) {
        LOGCATE("pkt alloc failed");
        goto End;
    }
    ret = avformat_write_header(ofmt, NULL);
    if (ret < 0) {
        LOGCATE("avformat_write_header failed :%s", av_err2str(ret));
        goto End;
    }

//    BitStreamFilterHelper* helper = new BitStreamFilterHelper;
//    helper->initAudio();
//    helper->initVideo();

    // start muxer

    for (;;) {
        AVFormatContext *ifmt_ctx;
        int stream_index = 0;
        AVStream *in_stream, *out_stream;

        if (av_compare_ts(cur_video_pts, imft_video->streams[videoIn_index]->time_base,
                          cur_audio_pts,
                          imft_audio->streams[audioIn_index]->time_base) <= 0) {
            // 视频略快
            tempifmt = imft_video;
            stream_index = videoOut_index;

            if (av_read_frame(tempifmt, pkt) >= 0) {
                do {
                    in_stream = tempifmt->streams[pkt->stream_index];
                    out_stream = ofmt->streams[stream_index];
//                    LOGCATE("log video current pkt index:%d",pkt->stream_index);
                    if (pkt->stream_index == videoIn_index) {
//                        LOGCATE("transcode video in");
//                        LOGCATE("log in timebase video:%d",in_stream->time_base);
                        // write pts
                        if (pkt->pts == AV_NOPTS_VALUE){
                            AVRational time_base1 = in_stream->time_base;
                            LOGCATE("log timebase-num:%d timebase-des:%d frame_rate-num:%d frame_rate_des:%d",
                                    time_base1.num,time_base1.den,in_stream->r_frame_rate.num,in_stream->r_frame_rate.den);
                            int64_t calc_duration =
                                    (double) AV_TIME_BASE / av_q2d(in_stream->r_frame_rate);
                            pkt->pts = (double) (frame_index * calc_duration) /
                                       (double) (av_q2d(time_base1) * AV_TIME_BASE);
                            int64_t temp = (double)frame_index / av_q2d(in_stream->r_frame_rate) * (time_base1.den / time_base1.num);
                            // pts:50049 calc_duration:41708
                            pkt->dts = pkt->pts;
                            pkt->duration = (double) calc_duration /
                                            (double) (av_q2d(time_base1) * AV_TIME_BASE);
                            LOGCATE("video ====== calc_duration:%lld pts:%lld dts:%lld duration:%lld frame_index:%d timebase:%lld temp-pts:%lld",
                                    calc_duration,pkt->pts,pkt->dts,pkt->duration,frame_index,temp);
                            frame_index++;
                        }
                        cur_video_pts = pkt->pts;
                        break;
                    }

                } while (av_read_frame(tempifmt, pkt) >= 0);
            } else {
//                LOGCATE("transcode video out");
                break;
            }

        } else {

//            LOGCATE("has enter audio frame");

            ifmt_ctx = imft_audio;
            stream_index = audioOut_index;
            if (av_read_frame(ifmt_ctx, pkt) >= 0) {
                do {
                    in_stream = ifmt_ctx->streams[pkt->stream_index];
                    out_stream = ofmt->streams[stream_index];
                    if (pkt->stream_index == audioIn_index) {
//                        LOGCATE("transcode audio in");
//                        LOGCATE("log in timebase audio:%d",in_stream->time_base);
                        //FIX：No PTS
                        //Simple Write PTS
                        if (pkt->pts == AV_NOPTS_VALUE) {
                            //Write PTS
                            AVRational time_base1 = in_stream->time_base;
                            //Duration between 2 frames (us)
                            int64_t calc_duration =
                                    (double) AV_TIME_BASE / av_q2d(in_stream->r_frame_rate);
                            //Parameters
                            pkt->pts = (double) (frame_index * calc_duration) /
                                       (double) (av_q2d(time_base1) * AV_TIME_BASE);
                            pkt->dts = pkt->pts;
                            pkt->duration = (double) calc_duration /
                                            (double) (av_q2d(time_base1) * AV_TIME_BASE);
                            frame_index++;
                            LOGCATE("audio ====== calc_duration:%lld pts:%lld dts:%lld duration:%lld frame_index:%d",
                                    calc_duration,pkt->pts,pkt->dts,pkt->duration,frame_index);
                        } else {
//                            LOGCATE(" audio don't need to setup timestamp");
                        }
                        cur_audio_pts = pkt->pts;
                        break;
                    }
                } while (av_read_frame(ifmt_ctx, pkt) >= 0);
            } else {
//                LOGCATE("transcode audio out");
                break;
            }
        }

        pkt->stream_index = stream_index;
        av_packet_rescale_ts(pkt,in_stream->time_base,out_stream->time_base);

//        LOGCATE("log stream_index:%d in_timeBase:%d out_timebase:%d",pkt->stream_index,
//                in_stream->time_base,out_stream->time_base);
        ret = av_interleaved_write_frame(ofmt,pkt);
        if (ret < 0){
            LOGCATE("av_interleaved_write_frame failed:%s ",av_err2str(ret));
            break;
        }
        av_packet_unref(pkt);
    }
    av_write_trailer(ofmt);

    End:
    LOGCATE("end all");
    if (pkt){
        av_packet_free(&pkt);
    }
    if (imft_audio) {
        avformat_close_input(&imft_audio);
        avformat_free_context(imft_audio);
    }
    if (imft_video) {
        avformat_close_input(&imft_video);
        avformat_free_context(imft_video);
    }

    if (ofmt){
        avformat_free_context(ofmt);
    }
    /* close output */
    if (ofmt && !(ofmt->flags & AVFMT_NOFILE))
        avio_close(ofmt->pb);

    if (ret < 0 && ret != AVERROR_EOF){
        LOGCATE("final something error ,delete file ");
        remove(outputFileUrl);
    }


    LOGCATE("all over");
}

int CustomMuxer::addNewStream(AVFormatContext *pContext, AVFormatContext *pContext1, int index,
                              int *outIndex) {
    int ret = 0;
    AVStream *stream = avformat_new_stream(pContext1, NULL);
    if (!stream) {
        LOGCATE("open audio input failed:%s", av_err2str(ret));
        ret = -1;
        return ret;
    }
    ret = avcodec_parameters_copy(stream->codecpar, pContext->streams[index]->codecpar);
    if (ret < 0) {
        LOGCATE("avcodec_parameters_copy audio failed :%s", av_err2str(ret));
        return ret;
    }
    stream->codecpar->codec_tag = 0;
    *outIndex = stream->index;
    return ret;
}

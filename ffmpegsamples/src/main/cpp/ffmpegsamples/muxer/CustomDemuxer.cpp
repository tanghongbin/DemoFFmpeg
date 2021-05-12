//
// Created by Admin on 2020/11/18.
//

#include <utils.h>
#include <cstdio>
#include <CustomGLUtils.h>

#include "CustomDemuxer.h"

extern "C"{
#include <libavformat/avformat.h>
}



/**
 * 复杂版本
 * @param path
 */
void CustomDemuxer::demuxerDiffcult(const char *path){
    LOGCATE("has enter demuxer:%s",path);
    int ret;
    const char * out_a_name = getRandomStr("mp3_",".aac","splitFiles/");
    const char * out_v_name = getRandomStr("h264_",".h264","splitFiles/");

    LOGCATE("log video_out_name:%s \n audio_out_name:%s",out_v_name,out_a_name);
    AVFormatContext * iFmtCtx;
    AVPacket* pkt;
    int audio_index = -1,video_index = -1;
    AVBitStreamFilterContext* h264bsfc;
    int frame_index = 0;
    AVFormatContext * out_audio_fmt;
    AVFormatContext * out_video_fmt;

    iFmtCtx = avformat_alloc_context();
    if (!iFmtCtx){
        LOGCATE("can't alloc formatcontext");
        return;
    }

    ret = avformat_open_input(&iFmtCtx,path,NULL,NULL);
    if (ret < 0){
        LOGCATE("can't open input file:%s",av_err2str(ret));
        goto End;
    }
    ret = avformat_find_stream_info(iFmtCtx,NULL);
    if (ret < 0){
        LOGCATE("can't find any stream info:%s",av_err2str(ret));
        goto End;
    }

    ret = avformat_alloc_output_context2(&out_audio_fmt,NULL,NULL,out_a_name);
    if (ret < 0){
        LOGCATE("can't alloc output audio ctx");
        goto End;
    }
    ret = avformat_alloc_output_context2(&out_video_fmt,NULL,NULL,out_v_name);
    if (ret < 0){
        LOGCATE("can't alloc output video ctx");
        goto End;
    }

    for (int i = 0; i < iFmtCtx->nb_streams; ++i) {
        AVStream* outStream;
        if (iFmtCtx->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_AUDIO){
            audio_index = i;
            outStream = avformat_new_stream(out_audio_fmt,NULL);
        } else if (iFmtCtx->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_VIDEO){
            video_index = i;
            outStream = avformat_new_stream(out_video_fmt,NULL);
        } else {
            break;
        }
        LOGCATE("log outstream:%p",outStream);
        ret = avcodec_parameters_copy(outStream->codecpar,iFmtCtx->streams[i]->codecpar);
        if (ret < 0){
            LOGCATE("copy params failed:%s",av_err2str(ret));
            goto End;
        }
        outStream->codecpar->codec_tag = 0;
    }


    //Dump Format------------------
    printf("\n==============Input Video=============\n");
    av_dump_format(iFmtCtx, 0, path, 0);
    printf("\n==============Output Video============\n");
    av_dump_format(out_audio_fmt, 0, out_a_name, 1);
    printf("\n==============Output Audio============\n");
    av_dump_format(out_video_fmt, 0, out_v_name, 1);

    // open output file
    if (!(out_audio_fmt->flags & AVFMT_NOFILE)) {
        if ((ret = avio_open(&out_audio_fmt->pb,out_a_name,AVIO_FLAG_WRITE)) < 0){
            LOGCATE("open audio output file failed:%s",av_err2str(ret));
            goto End;
        }
    }

    if (!(out_video_fmt->flags & AVFMT_NOFILE)) {
        if ((ret = avio_open(&out_video_fmt->pb,out_v_name,AVIO_FLAG_WRITE)) < 0){
            LOGCATE("open video output file failed:%s",av_err2str(ret));
            goto End;
        }
    }

    LOGCATE("log audio_index:%d video_index:%d",audio_index,video_index);
    pkt = av_packet_alloc();
    if (!pkt){
        LOGCATE("can't alloc pkt");
        goto End;
    }

    h264bsfc =  av_bitstream_filter_init("h264_mp4toannexb");
    LOGCATE("all prepare over,start read frame");

    ret = avformat_write_header(out_audio_fmt,NULL);
    if (ret < 0){
        LOGCATE("avformat_write_header out_audio_fmt failed:%s",av_err2str(ret));
        goto End;
    }

    ret = avformat_write_header(out_video_fmt,NULL);
    if (ret < 0){
        LOGCATE("avformat_write_header out_video_fmt failed:%s",av_err2str(ret));
        goto End;
    }

    for(;;){
        AVFormatContext * tempOutFmt;
        AVStream* inStream,*outStream;
        ret = av_read_frame(iFmtCtx,pkt);
        if (ret < 0){
            LOGCATE("read frame has finished:%s",av_err2str(ret));
            break;
        }
        inStream = iFmtCtx->streams[pkt->stream_index];
        // 过滤无效frame
//        LOGCATE("start read mediatype ,current index:%d",pkt->stream_index);
        AVMediaType mediaType = iFmtCtx->streams[pkt->stream_index]->codecpar->codec_type;
        if (mediaType != AVMEDIA_TYPE_VIDEO &&
                mediaType != AVMEDIA_TYPE_AUDIO &&
                mediaType != AVMEDIA_TYPE_SUBTITLE){
            av_packet_unref(pkt);
            continue;
        }
        if (pkt -> stream_index == audio_index){
            tempOutFmt = out_audio_fmt;
            outStream = out_audio_fmt->streams[0];
        } else if (pkt->stream_index == video_index){
            tempOutFmt = out_video_fmt;
            outStream = out_video_fmt->streams[0];
            av_bitstream_filter_filter(h264bsfc, iFmtCtx->streams[video_index]->codec, NULL, &pkt->data,
                                       &pkt->size, pkt->data, pkt->size, 0);
        }
        frame_index++;
        pkt->stream_index = 0;
//        LOGCATE("start resacle");
        av_packet_rescale_ts(pkt,inStream->time_base,
                outStream->time_base);
//        LOGCATE("current frame:%d",frame_index);
        av_interleaved_write_frame(tempOutFmt,pkt);
        av_packet_unref(pkt);
    }

    av_write_trailer(out_audio_fmt);
    av_write_trailer(out_video_fmt);

    End:

    av_packet_unref(pkt);
    av_bitstream_filter_close(h264bsfc);


    if (pkt != nullptr){
        av_packet_free(&pkt);
    }


    if (out_audio_fmt && !(out_audio_fmt->oformat->flags & AVFMT_NOFILE))
        avio_close(out_audio_fmt->pb);

    if (out_video_fmt && !(out_video_fmt->oformat->flags & AVFMT_NOFILE))
        avio_close(out_video_fmt->pb);

    avformat_free_context(out_audio_fmt);
    avformat_free_context(out_video_fmt);

    LOGCATE("release _over");
    avformat_close_input(&iFmtCtx);

    LOGCATE("release _over2");
    if (ret < 0 && ret != AVERROR_EOF){
        remove(out_a_name);
        remove(out_v_name);
    }
    LOGCATE("free all context");
}

void CustomDemuxer::demuxer(const char *path) {
    LOGCATE("has enter demuxer:%s",path);
    int ret;
    const char * out_a_name = getRandomStr("",".mp3","splitFiles/");
    const char * out_v_name = getRandomStr("",".h264","splitFiles/");
    FILE* out_audio;
    FILE* out_video;
    LOGCATE("log video_out_name:%s \n audio_out_name:%s",out_v_name,out_a_name);
    AVFormatContext * iFmtCtx;
    AVPacket* pkt;
    int audio_index = -1,video_index = -1;
    const AVBitStreamFilter *bitFilter;
    AVBSFContext* avbsfContext;
    AVBitStreamFilterContext* h264bsfc;
    int frame_index = 0;

    iFmtCtx = avformat_alloc_context();
    if (!iFmtCtx){
        LOGCATE("can't alloc formatcontext");
        return;
    }
    out_audio = fopen(out_a_name,"wb");
    if (!out_audio){
        LOGCATE("can't open audio file");
        ret = -1;
        goto End;
    }
    out_video = fopen(out_v_name,"wb");
    if (!out_video){
        LOGCATE("can't open video file");
        ret = -1;
        goto End;
    }
    ret = avformat_open_input(&iFmtCtx,path,NULL,NULL);
    if (ret < 0){
        LOGCATE("can't open input file:%s",av_err2str(ret));
        goto End;
    }
    ret = avformat_find_stream_info(iFmtCtx,NULL);
    if (ret < 0){
        LOGCATE("can't find any stream info:%s",av_err2str(ret));
        goto End;
    }
    for (int i = 0; i < iFmtCtx->nb_streams; ++i) {
        if (iFmtCtx->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_AUDIO){
            audio_index = i;
        } else if (iFmtCtx->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_VIDEO){
            video_index = i;
        }
    }
    LOGCATE("log audio_index:%d video_index:%d",audio_index,video_index);
    pkt = av_packet_alloc();
    if (!pkt){
        LOGCATE("can't alloc pkt");
        goto End;
    }
//    av_init_packet(pkt);

// 不使用这个，用法有问题，写出来的数据不能播放
//    bitFilter = av_bsf_get_by_name("h264_mp4toannexb");
//    if (!bitFilter){
//        LOGCATE("can't find bitfilter");
//        goto End;
//    }
//    ret = av_bsf_alloc(bitFilter,&avbsfContext);
//    if (ret < 0){
//        LOGCATE("av_bsf_alloc failed :%s",av_err2str(ret));
//        goto End;
//    }
//    avbsfContext->par_in = iFmtCtx->streams[video_index]->codecpar;
//    avbsfContext->par_out = iFmtCtx->streams[video_index]->codecpar;
//    avbsfContext->time_base_in = iFmtCtx->streams[video_index]->time_base;
//    avbsfContext->time_base_out = iFmtCtx->streams[video_index]->time_base;
//
//    ret = av_bsf_init(avbsfContext);
//    if (ret < 0){
//        LOGCATE("av_bsf_init failed :%s",av_err2str(ret));
//        goto End;
//    }

    h264bsfc =  av_bitstream_filter_init("h264_mp4toannexb");
    LOGCATE("all prepare over,start read frame");

    for(;;){
        ret = av_read_frame(iFmtCtx,pkt);
        if (ret < 0){
            LOGCATE("read frame has finished:%s",av_err2str(ret));
            break;
        }
        if (pkt -> stream_index == audio_index){
            fwrite(pkt->data,1,pkt->size,out_audio);
        } else if (pkt->stream_index == video_index){
            av_bitstream_filter_filter(h264bsfc, iFmtCtx->streams[video_index]->codec, NULL, &pkt->data,
                    &pkt->size, pkt->data, pkt->size, 0);
            fwrite(pkt->data,1,pkt->size,out_video);
        }
        frame_index++;
        LOGCATE("current frame:%d",frame_index);
        av_packet_unref(pkt);
    }


    End:

    av_packet_unref(pkt);
    av_bitstream_filter_close(h264bsfc);


    if (pkt != nullptr){
        av_packet_free(&pkt);
    }

    LOGCATE("release _over");
    avformat_close_input(&iFmtCtx);
    if (out_audio) fclose(out_audio);
    if (out_video) fclose(out_video);

    LOGCATE("release _over2");
    if (ret < 0 && ret != AVERROR_EOF){
        remove(out_a_name);
        remove(out_v_name);
    }
    LOGCATE("free all context");
}
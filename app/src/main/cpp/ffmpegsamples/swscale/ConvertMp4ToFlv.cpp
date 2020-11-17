//
// Created by Admin on 2020/11/16.
//

#include <utils.h>
#include <CustomGLUtils.h>
#include "ConvertMp4ToFlv.h"

extern "C"{
#include <libavformat/avformat.h>
}


const char *ConvertMp4ToFlv::convert(const char *path) {
    int ret = 0;
    const char *ERROR_PATH = "";
    const char *outputName = getRandomStr("video-", ".mkv", CONVERT_FORMAT_FILES);
    AVFormatContext *inputFormatCtx;
    AVFormatContext *outputFormatCtx;
    AVOutputFormat *ofmt;
    LOGCATE("log input url :%s", path);
    AVPacket *pkt;
    int frame_index = 0;

    inputFormatCtx = avformat_alloc_context();
    if (!inputFormatCtx){
        LOGCATE("can't alloc input fromat");
        ret = -1;
        goto End;
    }

    ret = avformat_open_input(&inputFormatCtx, path, NULL, NULL);
    if (ret < 0) {
        LOGCATE("can't open input :%s",av_err2str(ret));
        ret = -1;
        goto End;
    }

    ret = avformat_alloc_output_context2(&outputFormatCtx, NULL, NULL, outputName);
    if (ret < 0) {
        LOGCATE("can't alloc output format ctx :%s",av_err2str(ret));
        ret = -1;
        goto End;
    }
    ret = avformat_find_stream_info(inputFormatCtx, NULL);
    if (ret < 0) {
        LOGCATE("can't find some valid stream info :%s",av_err2str(ret));
        ret = -1;
        goto End;
    }

    pkt = av_packet_alloc();

    if (!pkt) {
        LOGCATE("can't alloc pkt");
        ret = -1;
        goto End;
    }

    av_init_packet(pkt);

    ofmt = outputFormatCtx->oformat;

    for (int i = 0; i < inputFormatCtx->nb_streams; ++i) {
        AVStream *inStream = inputFormatCtx->streams[i];
        AVStream *outStream = avformat_new_stream(outputFormatCtx, inStream->codec->codec);

        if (!outStream) {
            LOGCATE("can't create stream");
            ret = -1;
            goto End;
        }
        outStream->time_base = inStream->time_base;
        ret = avcodec_parameters_to_context(outStream->codec, inStream->codecpar);
        if (checkNegativeReturn(ret, "attach params failed")) {
            goto End;
        }
        outStream->codec->codec_tag = 0;
        if (outputFormatCtx->oformat->flags & AVFMT_GLOBALHEADER)
            outStream->codec->flags |= AV_CODEC_FLAG_GLOBAL_HEADER;
    }


    av_dump_format(outputFormatCtx, 0, outputName, 1);


    //打开输出文件（Open output file）
    if (!(ofmt->flags & AVFMT_NOFILE)) {
        ret = avio_open(&outputFormatCtx->pb, outputName, AVIO_FLAG_WRITE);
        if (ret < 0) {
            LOGCATE( "Could not open output file '%s'", outputName);
            goto End;
        }
    }

    LOGCATE("output format finished, prepare write header");

    //写文件头（Write file header）
    ret = avformat_write_header(outputFormatCtx, NULL);

    if (ret < 0){
        LOGCATE("can't write header:%s",av_err2str(ret));
        ret = -1;
        goto End;
    }

    LOGCATE("output format finished, prepare write frame");

    for (;;) {
        ret = av_read_frame(inputFormatCtx, pkt);
        if (ret < 0) {
            LOGCATE("read pkt has finished :%s  value:%d", av_err2str(ret),ret);
            break;
        }
//        LOGCATE("scale pkt value outputfotmat:%p",outputFormatCtx);
        av_packet_rescale_ts(pkt,
                             inputFormatCtx->streams[pkt->stream_index]->time_base,
                             outputFormatCtx->streams[pkt->stream_index]->time_base);


//        LOGCATE("real write outputfotmat:%p",outputFormatCtx);
        ret = av_interleaved_write_frame(outputFormatCtx, pkt);
//        LOGCATE("muxing result:%s", av_err2str(ret));
        if (ret < 0) {
            LOGCATE("error muxing:%s", av_err2str(ret));
            break;
        }
        av_packet_unref(pkt);
        frame_index++;
        LOGCATE("current frame index:%d", frame_index);
    }
    av_write_trailer(outputFormatCtx);

    End:

    if (pkt){
        av_packet_unref(pkt);
        av_packet_free(&pkt);
    }

    avformat_close_input(&inputFormatCtx);
    /* close output */
    if (outputFormatCtx && !(ofmt->flags & AVFMT_NOFILE))
        avio_close(outputFormatCtx->pb);
    avformat_free_context(outputFormatCtx);
    if (ret < 0 && ret != AVERROR_EOF) {
        LOGCATE("Error occurred.\n");
        remove(outputName);
        return ERROR_PATH;
    }

    LOGCATE("打印最终地址:%s ret:%d", outputName,ret);
    return outputName;
}


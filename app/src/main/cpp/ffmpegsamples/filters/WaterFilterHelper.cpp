//
// Created by Admin on 2020/11/10.
//



#include <CustomGLUtils.h>
#include <encode/EncodeYuvToJpg.h>
#include <iostream>
#include "WaterFilterHelper.h"

#include<sstream>

#include<iostream>
#include<string>
#include<cstdlib>
#include<sstream>
#include<stdio.h>
#include <cstring>
#include <typeinfo>


extern "C"{
#include <libavfilter/buffersink.h>
#include <libavutil/imgutils.h>
#include <libavfilter/buffersrc.h>
}

using namespace std;


int WaterFilterHelper::initWaterFilter(int in_width,int in_height) {
    char args[512];
    int ret;
    const AVFilter *buffersrc  = avfilter_get_by_name("buffer");
    const AVFilter *buffersink = avfilter_get_by_name("buffersink");
    AVFilterInOut *outputs = avfilter_inout_alloc();
    AVFilterInOut *inputs  = avfilter_inout_alloc();
    enum AVPixelFormat pix_fmts[] = { AV_PIX_FMT_YUV420P, AV_PIX_FMT_NONE };
    AVBufferSinkParams *buffersink_params;

    filter_graph = avfilter_graph_alloc();

    snprintf(args, sizeof(args),
             "video_size=%dx%d:pix_fmt=%d:time_base=%d/%d:pixel_aspect=%d/%d",
             in_width,in_height,AV_PIX_FMT_YUV420P,
             1, 25,1,1);

    ret = avfilter_graph_create_filter(&buffersrc_ctx, buffersrc, "in",
                                       args, NULL, filter_graph);
    if (ret < 0) {
        LOGCATE("Cannot create buffer source:%s",av_err2str(ret));
        return ret;
    }

    /* buffer video sink: to terminate the filter chain. */
    buffersink_params = av_buffersink_params_alloc();
    buffersink_params->pixel_fmts = pix_fmts;
    ret = avfilter_graph_create_filter(&buffersink_ctx, buffersink, "out",
                                       NULL, buffersink_params, filter_graph);
    av_free(buffersink_params);
    if (ret < 0) {
        LOGCATE("Cannot create buffer sink:%s",av_err2str(ret));
        return ret;
    }

    /* Endpoints for the filter graph. */
    outputs->name       = av_strdup("in");
    outputs->filter_ctx = buffersrc_ctx;
    outputs->pad_idx    = 0;
    outputs->next       = NULL;

    inputs->name       = av_strdup("out");
    inputs->filter_ctx = buffersink_ctx;
    inputs->pad_idx    = 0;
    inputs->next       = NULL;

//    const char *filter_descr = "lutyuv='u=128:v=128'";
//    const char *filter_descr = "boxblur";
//    const char *filter_descr = "hflip";
    //const char *filter_descr = "hue='h=60:s=-3'";
    const char *filter_descr = "crop=2/3*in_w:2/3*in_h";
//    const char *filter_descr = "scale=78:24";
//    const char *filter_descr = "drawbox=x=100:y=100:w=100:h=100:color=pink@0.5";
    //const char *filter_descr = "drawtext=fontfile=arial.ttf:fontcolor=green:fontsize=30:text='Lei Xiaohua'";


    if ((ret = avfilter_graph_parse_ptr(filter_graph, filter_descr,
                                        &inputs, &outputs, NULL)) < 0){
        LOGCATE("avfilter_graph_parse_ptr error:%s",av_err2str(ret));
        return ret;
    }


    if ((ret = avfilter_graph_config(filter_graph, NULL)) < 0){
        LOGCATE("avfilter_graph_config error:%s",av_err2str(ret));
        return ret;
    }
    return ret;
}


void WaterFilterHelper::unInit() {
    avfilter_graph_free(&filter_graph);
}

const char * WaterFilterHelper::testFilter(const char * inFileName){
    int ret = 0;
    const char * Error_Result = "";
    long long start = GetSysCurrentTime();

    // input yuv
    FILE* inFile = NULL;
    inFile = fopen(inFileName, "rb+");
    if (!inFile) {
        LOGCATE("Fail to open file\n");
        return Error_Result;
    }

    int in_width = 480;
    int in_height = 272;

    // output yuv
    FILE* outFile = NULL;


    const char * outFileName =  getRandomStr("filteryuv-",".yuv");
    LOGCATE("log outputName:%s",outFileName);
    outFile = fopen(outFileName, "wb");
    if (!outFile) {
        LOGCATE("Fail to create file for output\n");
        return Error_Result;
    }

    AVFilterGraph* filter_graph = avfilter_graph_alloc();
    if (!filter_graph) {
        LOGCATE("Fail to create filter graph!\n");
        return Error_Result;
    }

    // source filter
    char args[512];



    snprintf(args, sizeof(args),
                "video_size=%dx%d:pix_fmt=%d:time_base=%d/%d:pixel_aspect=%d/%d",
                in_width, in_height, AV_PIX_FMT_YUV420P,
                1, 25, 1, 1);

    const AVFilter* bufferSrc = avfilter_get_by_name("buffer");
    AVFilterContext* bufferSrc_ctx;

    ret = avfilter_graph_create_filter(&bufferSrc_ctx, bufferSrc, "in", args, NULL, filter_graph);
    if (ret < 0) {
        LOGCATE("Fail to create filter bufferSrc :%s",av_err2str(ret));
        return Error_Result;
    }

    // sink filter
    AVBufferSinkParams *bufferSink_params;
    AVFilterContext* bufferSink_ctx;
    const AVFilter* bufferSink = avfilter_get_by_name("buffersink");
    enum AVPixelFormat pix_fmts[] = { AV_PIX_FMT_YUV420P, AV_PIX_FMT_NONE };
    bufferSink_params = av_buffersink_params_alloc();
    bufferSink_params->pixel_fmts = pix_fmts;
    ret = avfilter_graph_create_filter(&bufferSink_ctx, bufferSink, "out", NULL, bufferSink_params, filter_graph);
    if (ret < 0) {
        LOGCATE("Fail to create filter sink filter\n");
        return Error_Result;
    }

    // split filter
    const AVFilter *splitFilter = avfilter_get_by_name("split");
    AVFilterContext *splitFilter_ctx;
    ret = avfilter_graph_create_filter(&splitFilter_ctx, splitFilter, "split", "outputs=2", NULL, filter_graph);
    if (ret < 0) {
        LOGCATE("Fail to create split filter:%s",av_err2str(ret));
        return Error_Result;
    }

    // crop filter
    const AVFilter *cropFilter = avfilter_get_by_name("crop");
    AVFilterContext *cropFilter_ctx;
    ret = avfilter_graph_create_filter(&cropFilter_ctx, cropFilter, "crop", "out_w=iw:out_h=ih/2:x=0:y=0", NULL, filter_graph);
    if (ret < 0) {
        LOGCATE("Fail to create crop filter\n");
        return Error_Result;
    }

    // vflip filter
    const AVFilter *vflipFilter = avfilter_get_by_name("vflip");
    AVFilterContext *vflipFilter_ctx;
    ret = avfilter_graph_create_filter(&vflipFilter_ctx, vflipFilter, "vflip", NULL, NULL, filter_graph);
    if (ret < 0) {
        LOGCATE("Fail to create vflip filter\n");
        return Error_Result;
    }

    // overlay filter
    const AVFilter *overlayFilter = avfilter_get_by_name("overlay");
    AVFilterContext *overlayFilter_ctx;
    ret = avfilter_graph_create_filter(&overlayFilter_ctx, overlayFilter, "overlay", "y=0:H/2", NULL, filter_graph);
    if (ret < 0) {
        LOGCATE("Fail to create overlay filter\n");
        return Error_Result;
    }

    // src filter to split filter
    ret = avfilter_link(bufferSrc_ctx, 0, splitFilter_ctx, 0);
    if (ret != 0) {
        LOGCATE("Fail to link src filter and split filter\n");
        return Error_Result;
    }
    // split filter's first pad to overlay filter's main pad
    ret = avfilter_link(splitFilter_ctx, 0, overlayFilter_ctx, 0);
    if (ret != 0) {
        LOGCATE("Fail to link split filter and overlay filter main pad\n");
        return Error_Result;
    }
    // split filter's second pad to crop filter
    ret = avfilter_link(splitFilter_ctx, 1, cropFilter_ctx, 0);
    if (ret != 0) {
        LOGCATE("Fail to link split filter's second pad and crop filter\n");
        return Error_Result;
    }
    // crop filter to vflip filter
    ret = avfilter_link(cropFilter_ctx, 0, vflipFilter_ctx, 0);
    if (ret != 0) {
        LOGCATE("Fail to link crop filter and vflip filter\n");
        return Error_Result;
    }
    // vflip filter to overlay filter's second pad
    ret = avfilter_link(vflipFilter_ctx, 0, overlayFilter_ctx, 1);
    if (ret != 0) {
        LOGCATE("Fail to link vflip filter and overlay filter's second pad\n");
        return Error_Result;
    }
    // overlay filter to sink filter
    ret = avfilter_link(overlayFilter_ctx, 0, bufferSink_ctx, 0);
    if (ret != 0) {
        LOGCATE("Fail to link overlay filter and sink filter\n");
        return Error_Result;
    }

    // check filter graph
    ret = avfilter_graph_config(filter_graph, NULL);
    if (ret < 0) {
        LOGCATE("Fail in filter graph\n");
        return Error_Result;
    }

//    char *graph_str = avfilter_graph_dump(filter_graph, NULL);
//    FILE* graphFile = NULL;
//    fopen_s(&graphFile, "graphFile.txt", "w");
//    fprintf(graphFile, "%s", graph_str);
//    av_free(graph_str);

    AVFrame *frame_in = av_frame_alloc();
    unsigned char *frame_buffer_in = (unsigned char *)av_malloc(av_image_get_buffer_size(AV_PIX_FMT_YUV420P, in_width, in_height, 1));
    av_image_fill_arrays(frame_in->data, frame_in->linesize, frame_buffer_in,
                         AV_PIX_FMT_YUV420P, in_width, in_height, 1);

    AVFrame *frame_out = av_frame_alloc();
    unsigned char *frame_buffer_out = (unsigned char *)av_malloc(av_image_get_buffer_size(AV_PIX_FMT_YUV420P, in_width, in_height, 1));
    av_image_fill_arrays(frame_out->data, frame_out->linesize, frame_buffer_out,
                         AV_PIX_FMT_YUV420P, in_width, in_height, 1);

    frame_in->width = in_width;
    frame_in->height = in_height;
    frame_in->format = AV_PIX_FMT_YUV420P;

    while (1) {

        if (fread(frame_buffer_in, 1, in_width*in_height * 3 / 2, inFile) != in_width*in_height * 3 / 2) {
            break;
        }
        //input Y,U,V
        frame_in->data[0] = frame_buffer_in;
        frame_in->data[1] = frame_buffer_in + in_width*in_height;
        frame_in->data[2] = frame_buffer_in + in_width*in_height * 5 / 4;

        if (av_buffersrc_add_frame(bufferSrc_ctx, frame_in) < 0) {
            LOGCATE("Error while add frame.\n");
            break;
        }

        /* pull filtered pictures from the filtergraph */
        ret = av_buffersink_get_frame(bufferSink_ctx, frame_out);
        if (ret < 0)
            break;

        //output Y,U,V
        if (frame_out->format == AV_PIX_FMT_YUV420P) {
            for (int i = 0; i < frame_out->height; i++) {
                fwrite(frame_out->data[0] + frame_out->linesize[0] * i, 1, frame_out->width, outFile);
            }
            for (int i = 0; i < frame_out->height / 2; i++) {
                fwrite(frame_out->data[1] + frame_out->linesize[1] * i, 1, frame_out->width / 2, outFile);
            }
            for (int i = 0; i < frame_out->height / 2; i++) {
                fwrite(frame_out->data[2] + frame_out->linesize[2] * i, 1, frame_out->width / 2, outFile);
            }
        }
        LOGCATE("Process 1 frame!\n");
        av_frame_unref(frame_out);
    }

    fclose(inFile);
    fclose(outFile);

    av_frame_free(&frame_in);
    av_frame_free(&frame_out);
    avfilter_graph_free(&filter_graph);

    EncodeYuvToJpg * encodeYuvToJpg = new EncodeYuvToJpg;
    const char * result = encodeYuvToJpg->encode(outFileName);
    LOGCATE("convert total cost:%lld ms",(GetSysCurrentTime() - start));
    return result;
}
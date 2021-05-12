//
// Created by Admin on 2020/11/10.
//

#ifndef DEMOC_WATERFILTERHELPER_H
#define DEMOC_WATERFILTERHELPER_H

extern "C"{
#include <libavfilter/avfilter.h>
#include <libavformat/avformat.h>
#include <libavutil/opt.h>
}

#define FILTER_DESC "scale=4:1"

/**
 * 水印帮助类
 */
class WaterFilterHelper {


public:

    AVFilterContext *buffersink_ctx;
    AVFilterContext *buffersrc_ctx;
    AVFilterGraph *filter_graph;


    int initWaterFilter(int in_width,int in_height);

    void unInit();

    const char * testFilter(const char * inputName);

//    AVFilter

};


#endif //DEMOC_WATERFILTERHELPER_H

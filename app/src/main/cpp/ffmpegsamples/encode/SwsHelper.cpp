//
// Created by Admin on 2020/11/4.
//



#include "SwsHelper.h"

extern "C"{
#include <libswscale/swscale.h>
#include <libavutil/opt.h>
}


void SwsHelper::init(int src_w, int src_h, enum AVPixelFormat srcFormat,
                          int dst_w, int dst_h, enum AVPixelFormat dstFormat,
                          int flags, SwsFilter *srcFilter,
                          SwsFilter *dstFilter, const double *param) {
    // method 1
    //Init Method 1
    mSwsCtx = sws_alloc_context();
    //Show AVOption
    av_opt_show2(mSwsCtx,stdout,AV_OPT_FLAG_VIDEO_PARAM,0);
    //Set Value
    av_opt_set_int(mSwsCtx,"sws_flags",SWS_BICUBIC|SWS_PRINT_INFO,0);
    av_opt_set_int(mSwsCtx,"srcw",src_w,0);
    av_opt_set_int(mSwsCtx,"srch",src_h,0);
    av_opt_set_int(mSwsCtx,"src_format",srcFormat,0);
    //'0' for MPEG (Y:0-235);'1' for JPEG (Y:0-255)
    av_opt_set_int(mSwsCtx,"src_range",1,0);
    av_opt_set_int(mSwsCtx,"dstw",dst_w,0);
    av_opt_set_int(mSwsCtx,"dsth",dst_h,0);
    av_opt_set_int(mSwsCtx,"dst_format",dstFormat,0);
    av_opt_set_int(mSwsCtx,"dst_range",1,0);
    sws_init_context(mSwsCtx,NULL,NULL);


    // method 2
//    mSwsCtx = sws_getContext(srcW,srcH,srcFormat,dstW,dstH,dstFormat,flags,srcFilter,
//            dstFilter,param);
}

int SwsHelper::scale( const uint8_t *const srcSlice[],
                      const int srcStride[], int srcSliceY, int srcSliceH,
                      uint8_t *const dst[], const int dstStride[]) {
   return sws_scale(mSwsCtx,srcSlice,srcStride,srcSliceY,srcSliceH,dst,dstStride);
}


void SwsHelper::unInit() {
    sws_freeContext(mSwsCtx);
}
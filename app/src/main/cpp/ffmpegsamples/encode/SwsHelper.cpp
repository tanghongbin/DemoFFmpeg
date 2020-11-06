//
// Created by Admin on 2020/11/4.
//


#include "SwsHelper.h"
extern "C"{
#include <libswscale/swscale.h>
}


void SwsHelper::init(int srcW, int srcH, enum AVPixelFormat srcFormat,
                          int dstW, int dstH, enum AVPixelFormat dstFormat,
                          int flags, SwsFilter *srcFilter,
                          SwsFilter *dstFilter, const double *param) {
    mSwsCtx = sws_getContext(srcW,srcH,srcFormat,dstW,dstH,dstFormat,flags,srcFilter,
            dstFilter,param);
}

int SwsHelper::scale( const uint8_t *const srcSlice[],
                      const int srcStride[], int srcSliceY, int srcSliceH,
                      uint8_t *const dst[], const int dstStride[]) {
   return sws_scale(mSwsCtx,srcSlice,srcStride,srcSliceY,srcSliceH,dst,dstStride);
}


void SwsHelper::unInit() {
    sws_freeContext(mSwsCtx);
}
//
// Created by Admin on 2020/11/4.
//

#ifndef DEMOC_SWSHELPER_H
#define DEMOC_SWSHELPER_H


extern "C"{
#include <libswscale/swscale.h>
};

class SwsHelper {

public:
    void init(int srcW, int srcH, enum AVPixelFormat srcFormat,
              int dstW, int dstH, enum AVPixelFormat dstFormat,
              int flags, SwsFilter *srcFilter,
              SwsFilter *dstFilter, const double *param);

    int scale( const uint8_t *const srcSlice[],
               const int srcStride[], int srcSliceY, int srcSliceH,
               uint8_t *const dst[], const int dstStride[]);

    void unInit();

    SwsContext *mSwsCtx;


private:

};


#endif //DEMOC_SWSHELPER_H

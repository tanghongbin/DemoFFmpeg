//
// Created by Admin on 2021/8/23.
//

#include <librtmp/include/rtmp.h>
#include "../../play_header/encoder/EncoderAACAndx264.h"
#include "x264.h"
#include "x264_config.h"

void EncoderAACAndx264::init(){
    mEncodeThreadV = new thread(EncoderAACAndx264::loopEncodeVideo, this);
}

void EncoderAACAndx264::loopEncodeVideo(EncoderAACAndx264* instance){

    int width = 720,height = 1280;
    int bitrate = 2000,maxBitrate = 2500;// 2000 kb
    long encodeStartTime = 0;
    int ret;
    x264_param_t params;
    x264_picture_t *pic_in = nullptr,*pic_out = nullptr;
    x264_t * encodeVHandle = nullptr;

    x264_param_default_preset(&params,"ultrafast","zerolatency");
    params.i_csp = X264_CSP_I420;
    params.i_width = width;
    params.i_height = height;
    params.i_keyint_max = 20;
    params.i_fps_num = 10;
    params.i_fps_den = 1;
    params.i_threads = 4;
    params.b_repeat_headers = 1;
    
    params.rc.i_bitrate = bitrate;
    params.rc.i_rc_method = X264_RC_CQP;
    params.rc.i_vbv_buffer_size = bitrate;
    params.rc.i_vbv_max_bitrate = maxBitrate;
    params.rc.b_mb_tree = 0;
    x264_param_apply_profile(&params,"baseline");
    encodeVHandle = x264_encoder_open(&params);
    pic_in = (x264_picture_t *) malloc(sizeof(x264_picture_t));
    pic_out = (x264_picture_t *) malloc(sizeof(x264_picture_t));
    
    // 初始化
    ret = x264_picture_alloc(pic_in,params.i_csp,params.i_width,params.i_height);
    if (ret == -1) {
        goto Encodex264End;
    }
    x264_picture_init(pic_out);
    encodeStartTime = RTMP_GetTime();
    
    while (instance -> isRunning) {
        NativeOpenGLImage *videoItem = instance->mVideoQueue.popFirst();
        // 假设这里的数据是yuv420p
        std::unique_lock<mutex> videoMutex(instance -> mAvMutex,std::defer_lock);
        memcpy(pic_in->img.plane[0],videoItem->ppPlane[0],params.i_width * params.i_height);
        memcpy(pic_in->img.plane[1],videoItem->ppPlane[1],params.i_width * params.i_height/4);
        memcpy(pic_in->img.plane[2],videoItem->ppPlane[2],params.i_width * params.i_height/4);
        int nNal = -1;
        x264_nal_t * nal = nullptr;

        if (!x264_encoder_encode(encodeVHandle,&nal,&nNal,pic_in,pic_out)){
            NativeOpenGLImageUtil::FreeNativeImage(videoItem);
            delete videoItem;
            goto Encodex264End;
        }
        pic_in->i_pts++;
        pic_out->i_pts++;
        int spsLen,ppsLen;
        uint8_t * sps,*pps;
        // 获取pps，sps，视频数据帧
        for (int i = 0; i < nNal; ++i) {
            if (nal[i].i_type == NAL_SPS) {
                spsLen = nal[i].i_payload - 4;
                sps = (uint8_t*) malloc((size_t)(spsLen + 1));
                memcpy(sps,nal[i].p_payload + 4,spsLen);
            } else if (nal[i].i_type == NAL_PPS) {
                ppsLen = nal[i].i_payload - 4;
                pps = (uint8_t*) malloc((size_t)(ppsLen + 1));
                memcpy(pps,nal[i].p_payload + 4,ppsLen);
                // 推送sps,pps信息
                instance -> addH264Header(sps,pps,spsLen,ppsLen);
            } else {
                instance -> addH264Body(nal[i].p_payload,nal[i].i_payload);
            }
        }
    }

    Encodex264End:
    if (pic_in) x264_picture_clean(pic_in);
    if (pic_out) x264_picture_clean(pic_out);
    x264_param_cleanup(&params);
    if (encodeVHandle) x264_encoder_close(encodeVHandle);
    
}

void EncoderAACAndx264::destroy(){
    if (mEncodeThreadV){
        mEncodeThreadV->join();
        delete mEncodeThreadV;
    }

    do {
        AudioRecordItemInfo *audioItem = mAudioQueue.popFirst();
        if (audioItem) audioItem->recycle();
    } while (mAudioQueue.size() > 0);
    
    do {
        NativeOpenGLImage *videoItem = mVideoQueue.popFirst();
        if (videoItem) { 
            NativeOpenGLImageUtil::FreeNativeImage(videoItem);
            delete videoItem;
        }
    } while (mVideoQueue.size() > 0);
}

void EncoderAACAndx264::encodeData(int type,uint8_t* data){

}

void EncoderAACAndx264::addH264Header(uint8_t *string, uint8_t *string1, int i, int i1) {

}

void EncoderAACAndx264::addH264Body(uint8_t *string, int i) {

}

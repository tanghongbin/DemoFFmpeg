//
// Created by Admin on 2021/8/23.
//

#include <rtmp/rtmp.h>
#include <x264/x264.h>
#include "../../play_header/encoder/EncoderAACAndx264.h"
#include <utils/CustomGLUtils.h>


void EncoderAACAndx264::init(){
    rtmpPushHelper = new RtmpPushHelper();
    rtmpPushHelper->initPush();
    mEncodeThreadV = new thread(EncoderAACAndx264::loopEncodeVideo, this);
}

void EncoderAACAndx264::loopEncodeVideo(EncoderAACAndx264* instance){

    int width = 720,height = 1280,fps = 25;
    int bitrate = 1200,maxBitrate = bitrate * 1.2 ;// 2000 kb
    long encodeStartTime = 0;
    int ret;
    x264_param_t params;
    x264_picture_t *pic_in = nullptr;
    x264_t * encodeVHandle = nullptr;

    ret = x264_param_default_preset(&params,x264_preset_names[0],x264_tune_names[7]);
    checkNegativeError(ret,"x264_param_default_preset");
    params.i_level_idc = 32;
    params.i_csp = X264_CSP_I420;
    params.i_width = width;
    params.i_height = height;
    params.i_bframe = 0; // i帧
    params.rc.i_rc_method = X264_RC_ABR;
    params.rc.i_bitrate = bitrate;
    params.rc.i_vbv_max_bitrate = maxBitrate;
    params.rc.i_vbv_buffer_size = bitrate;
    // 帧率
    params.i_fps_den = 1;
    params.i_fps_num = fps;
    params.i_timebase_den = params.i_fps_num;
    params.i_timebase_num = params.i_fps_den;
    params.b_vfr_input = 0;
    //帧距离(关键帧)  2s一个关键帧
    params.i_keyint_max = fps * 2;
    // 是否复制sps和pps放在每个关键帧的前面 该参数设置是让每个关键帧(I帧)都附带sps/pps。
    params.b_repeat_headers = 1;
    params.i_threads = 1;

    x264_param_apply_profile(&params, "baseline");
    //打开编码器
    checkNegativeError(ret,"x264_param_apply_profile");
    encodeVHandle = x264_encoder_open(&params);
    if (!encodeVHandle){
        LOGCATE("编码器打开失败");
        goto Encodex264End;
    }
    LOGCATE("编码器打开成功");
    pic_in = (x264_picture_t *) malloc(sizeof(x264_picture_t));

    // 初始化
    ret = x264_picture_alloc(pic_in,params.i_csp,params.i_width,params.i_height);
    if (ret == -1) {
        goto Encodex264End;
    }
    encodeStartTime = RTMP_GetTime();
    
    while (instance -> isRunning) {
        NativeOpenGLImage *videoItem = instance->mVideoQueue.popFirst();
        if (videoItem == nullptr) {
            usleep(1000 * 10);
            continue;
        }
        // 假设这里的数据是yuv420p
        std::unique_lock<mutex> videoMutex(instance -> mAvMutex,std::defer_lock);
        memcpy(pic_in->img.plane[0],videoItem->ppPlane[0],params.i_width * params.i_height);
        memcpy(pic_in->img.plane[1],videoItem->ppPlane[1],params.i_width * params.i_height/4);
        memcpy(pic_in->img.plane[2],videoItem->ppPlane[2],params.i_width * params.i_height/4);
        int nNal = -1;
        x264_nal_t* nal;

        NativeOpenGLImageUtil::FreeNativeImage(videoItem);
        delete videoItem;
        x264_picture_t pic_out;
        if (!x264_encoder_encode(encodeVHandle,&nal,&nNal,pic_in,&pic_out)){
            goto Encodex264End;
        }
        pic_in->i_pts++;
        int spsLen = 0,ppsLen = 0;
        uint8_t sps[100];
        uint8_t pps[100];
        // 获取pps，sps，视频数据帧
        for (int i = 0; i < nNal; ++i) {
            if (nal[i].i_type == NAL_SPS) {
                spsLen = nal[i].i_payload - 4;
                memcpy(sps,nal[i].p_payload + 4,spsLen);
            } else if (nal[i].i_type == NAL_PPS) {
                ppsLen = nal[i].i_payload - 4;
                memcpy(pps,nal[i].p_payload + 4,ppsLen);
                // 推送sps,pps信息
                instance -> addH264Header(sps,pps,spsLen,ppsLen);
            } else {
                instance -> addH264Body(nal[i].i_type,nal[i].p_payload,nal[i].i_payload);
            }
        }
    }

    Encodex264End:
    if (encodeVHandle) x264_encoder_close(encodeVHandle);
    if (pic_in) {
        x264_picture_clean(pic_in);
    }
    LOGCATE("loop is over");
}

void EncoderAACAndx264::destroy(){
    isRunning = false;
    mVideoQueue.pushLast(nullptr);
    mAudioQueue.pushLast(nullptr);
    if (mEncodeThreadV){
        mEncodeThreadV->join();
        delete mEncodeThreadV;
    }

    mVideoQueue.pushLast(nullptr);
    mAudioQueue.pushLast(nullptr);
    do {
        AudioRecordItemInfo *audioItem = mAudioQueue.popFirst();
        if (audioItem) audioItem->recycle();
    } while (mAudioQueue.size() > 0);
    LOGCATE("encode prepare delete all mAudioQueue");
    do {
        NativeOpenGLImage *videoItem = mVideoQueue.popFirst();
        if (videoItem) { 
            NativeOpenGLImageUtil::FreeNativeImage(videoItem);
            delete videoItem;
        }
    } while (mVideoQueue.size() > 0);
    LOGCATE("encode prepare delete all mVideoQueue");
    if (rtmpPushHelper){
        rtmpPushHelper->destroy();
        delete rtmpPushHelper;
        rtmpPushHelper = nullptr;
    }
    LOGCATE("encode aac and h264 all over");
}

void EncoderAACAndx264::putAudioData(unsigned char * data, int length) {

}

void EncoderAACAndx264::putVideoData(NativeOpenGLImage* data){
    if (!isRunning) return;
    mVideoQueue.pushLast(data);
}

void EncoderAACAndx264::addH264Header(uint8_t *sps, uint8_t *pps, int spsLen, int ppsLen) {
    rtmpPushHelper->putVideoSpsPps(sps,pps,spsLen,ppsLen);
}

void EncoderAACAndx264::addH264Body(int type,uint8_t *payload, int i_payload) {
    rtmpPushHelper->putVideoBody(type,payload,i_payload);
}

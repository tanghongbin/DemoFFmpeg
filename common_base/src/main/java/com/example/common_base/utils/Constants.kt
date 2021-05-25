package com.example.common_base.utils

import android.os.Environment

object Constants {
    /*****
     * /storage/emulated/0/ffmpegtest/filterImg/test1.jpg
     */
    val FILD_DIR = "/ffmpegtest"
    val FILD_VIDEO_DIR = "/ffmpegtest/videos"
    val FLV_ONLINE = "http://liteavapp.qcloud.com/live/liteavdemoplayerstreamid.flv"
    val RTMP_WINDOW_PUSH = "rtmp://106.13.78.235:1935/demo/windowsPush"
    val RTMP_ANDROID_PUSH = "rtmp://106.13.78.235:1935/demo/androidRtmpPush"
    val MP4_ONLINE_2 = "http://freesaasofficedata.oss.aliyuncs.com/2222/20190930142639768.mp4"
    val MP4_ONLINE = "http://pweb.longshinewz.com/Attachment/MsgFile/a33c1c6032ec4862b44728bde62b6a05.mp4"
    val MP4_PATH = Environment.getExternalStorageDirectory().absolutePath + "/test.mkv"
    val MP4_TEST6_PATH = Environment.getExternalStorageDirectory().absolutePath + "$FILD_VIDEO_DIR/test6.mp4"
    val MP4_PORTAIT_PATH = Environment.getExternalStorageDirectory().absolutePath + "$FILD_VIDEO_DIR/test5.mp4"
    val MP4_PLAY_PATH = Environment.getExternalStorageDirectory().absolutePath + "$FILD_VIDEO_DIR/video.mp4"
    val MP4_PLAY_BIG_PATH = Environment.getExternalStorageDirectory().absolutePath + "$FILD_VIDEO_DIR/blackanimal.mp4"
    val SLIGHT_MP4_VIDEO_PATH = Environment.getExternalStorageDirectory().absolutePath + "$FILD_VIDEO_DIR/test5.mp4"
    val YUV_PATH = Environment.getExternalStorageDirectory().absolutePath + "$FILD_DIR/YUV_Image_840x1074.NV21"
    val ENCODE_VIDEO_PATH = Environment.getExternalStorageDirectory().absolutePath + "$FILD_VIDEO_DIR/encodeVideo.h264"

    val FILTER_ADD_TO_YUV_PATH = Environment.getExternalStorageDirectory().absolutePath + "$FILD_VIDEO_DIR/filterAddToYuv.h264"
    val SWS_CONVERT_PNG = Environment.getExternalStorageDirectory().absolutePath + "/sws_after_scale.png"
    val TEST_JPG = Environment.getExternalStorageDirectory().absolutePath + "$FILD_DIR/filterImg/test1.jpg"
    val TEST_PNG = Environment.getExternalStorageDirectory().absolutePath + "$FILD_DIR/filterImg/test2.png"


    // =================== download -=====================================
    val MP4_DOWNLOAD_FUNNY_VIDEO = Environment.getExternalStorageDirectory().absolutePath + "$FILD_DIR/funny_video.mp4"
    val FLV_DOWNLOAD_MY_LOGO = Environment.getExternalStorageDirectory().absolutePath + "$FILD_DIR/my_logo.flv"

    val HW_MEDIA_CODEC_TIMEOUT = 12000
    val MAX_VIDEO_SEEK_BAR_PROGRESS = 100

    val TIME_UNIT_US = 1000 * 1000L
    val TIME_UNIT_MS = TIME_UNIT_US / 1000L
    val TIME_UNIT_SEC = TIME_UNIT_MS / 1000L
}
package com.testthb.common_base.utils

import android.os.Environment
import com.testthb.common_base.DemoApplication

object Constants {
    /*****
     * /storage/emulated/0/ffmpegtest/filterImg/test1.jpg
     */
    val FILD_DIR = "${DemoApplication.getInstance().filesDir.absoluteFile}/ffmpegtest"
    val FILD_VIDEO_DIR =  "${FILD_DIR}/videos"
    val IMG_DIR =Environment.getExternalStorageDirectory().absolutePath + "/ffmpegtest/img"
    val FLV_ONLINE = "http://liteavapp.qcloud.com/live/liteavdemoplayerstreamid.flv"
    val RTMP_WINDOW_PUSH = "rtmp://106.13.78.235:1935/demo/windowsPush"
    val RTMP_ANDROID_PUSH = "rtmp://106.13.78.235:1935/demo/androidRtmpPush"
    val MP4_ONLINE_2 = "http://freesaasofficedata.oss.aliyuncs.com/2222/20190930142639768.mp4"
    val MP4_ONLINE = "http://pweb.longshinewz.com/Attachment/MsgFile/a33c1c6032ec4862b44728bde62b6a05.mp4"
    val MP4_PATH = Environment.getExternalStorageDirectory().absolutePath + "/test.mkv"
    val MP4_TEST6_PATH = "$FILD_VIDEO_DIR/test6.mp4"
    val MP4_PORTAIT_PATH =   "$FILD_VIDEO_DIR/test5.mp4"
    val MP4_PLAY_PATH =  "$FILD_VIDEO_DIR/video.mp4"
    val MP4_PLAY_BIG_PATH =  "$FILD_VIDEO_DIR/blackanimal.mp4"
    val MKV_PLAY_BIG_PATH =  "$FILD_VIDEO_DIR/test-movie.mkv"
    val SLIGHT_MP4_VIDEO_PATH =  "$FILD_VIDEO_DIR/test5.mp4"
    val YUV_PATH =  "$FILD_DIR/YUV_Image_840x1074.NV21"
    val ENCODE_VIDEO_PATH =  "$FILD_VIDEO_DIR/encodeVideo.h264"

    val FILTER_ADD_TO_YUV_PATH =  "$FILD_VIDEO_DIR/filterAddToYuv.h264"
    val SWS_CONVERT_PNG =  "/sws_after_scale.png"
    val TEST_JPG =  "$FILD_DIR/filterImg/test1.jpg"
    val TEST_PNG =  "$FILD_DIR/filterImg/test2.png"

    val bugly_id = "0c9dacacfc"


    // =================== download -=====================================
    val MP4_DOWNLOAD_FUNNY_VIDEO =  "$FILD_DIR/funny_video.mp4"
    val FLV_DOWNLOAD_MY_LOGO =  "$FILD_DIR/my_logo.flv"

    val HW_MEDIA_CODEC_TIMEOUT = 12000
    val MAX_VIDEO_SEEK_BAR_PROGRESS = 100

    val TIME_UNIT_US = 1000 * 1000L
    val TIME_UNIT_MS = TIME_UNIT_US / 1000L
    val TIME_UNIT_SEC = TIME_UNIT_MS / 1000L
}
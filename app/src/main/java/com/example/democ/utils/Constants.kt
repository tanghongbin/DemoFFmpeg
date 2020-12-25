package com.example.democ.utils

import android.os.Environment

object Constants {
    val FILD_DIR = "/ffmpegtest"
    val MP4_ONLINE_2 = "http://freesaasofficedata.oss.aliyuncs.com/2222/20190930142639768.mp4"
    val MP4_ONLINE = "http://pweb.longshinewz.com/Attachment/MsgFile/a33c1c6032ec4862b44728bde62b6a05.mp4"
    val MP4_PATH = Environment.getExternalStorageDirectory().absolutePath + "${FILD_DIR}/test.mp4"
    val MP4_PLAY_PATH = Environment.getExternalStorageDirectory().absolutePath + "${FILD_DIR}/video.mp4"
    val MP4_PLAY_BIG_PATH = Environment.getExternalStorageDirectory().absolutePath + "${FILD_DIR}/blackanimal.mp4"
    val SLIGHT_MP4_VIDEO_PATH = Environment.getExternalStorageDirectory().absolutePath + "${FILD_DIR}/test5.mp4"
    val YUV_PATH = Environment.getExternalStorageDirectory().absolutePath + "${FILD_DIR}/YUV_Image_840x1074.NV21"
    val ENCODE_VIDEO_PATH = Environment.getExternalStorageDirectory().absolutePath + "${FILD_DIR}/encodeVideo.h264"

    val FILTER_ADD_TO_YUV_PATH = Environment.getExternalStorageDirectory().absolutePath + "${FILD_DIR}/filterAddToYuv.h264"
    val SWS_CONVERT_PNG = Environment.getExternalStorageDirectory().absolutePath + "/sws_after_scale.png"


    // =================== download -=====================================
    val MP4_DOWNLOAD_FUNNY_VIDEO = Environment.getExternalStorageDirectory().absolutePath + "${FILD_DIR}/funny_video.mp4"
    val FLV_DOWNLOAD_MY_LOGO = Environment.getExternalStorageDirectory().absolutePath + "${FILD_DIR}/my_logo.flv"
}
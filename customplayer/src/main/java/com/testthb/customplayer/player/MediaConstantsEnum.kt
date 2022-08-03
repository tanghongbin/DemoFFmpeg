package com.testthb.customplayer.player

enum class MediaConstantsEnum(val value:Int) {
    // 播放器，录制合成器
    NONE(0),
    MEDIA_PLAYER(1),
    MEDIA_MUXER(2),
    //  1-ffmpeg 编码,2-硬编码,3-直播推流,4-短视频录制
    MUXER_RECORD_FFMPEG(11),MUXER_RECORD_HW(12),MUXER_RTMP(13),MUXER_SHORT_VIDEO(14),
    // 1- ffmpeg 播放，2 - 硬解码
    PLAYER_FFMPEG(21),PLAYER_HW(22)
}
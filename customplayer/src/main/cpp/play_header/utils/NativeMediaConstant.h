//
// Created by Admin on 2022/8/2.
//

#ifndef DEMOFFMPEG_NATIVEMEDIACONSTANT_H
#define DEMOFFMPEG_NATIVEMEDIACONSTANT_H

#define NONE 0
#define MEDIA_PLAYER 1
#define MEDIA_MUXER 2
//  1-ffmpeg 编码,2-硬编码,3-直播推流,4-短视频录制
#define MUXER_RECORD_FFMPEG 11
#define MUXER_RECORD_HW 12
#define MUXER_RTMP 13
#define MUXER_SHORT_VIDEO 14
// 1- ffmpeg 播放，2 - 硬解码
#define PLAYER_FFMPEG 21
#define PLAYER_HW 22

#endif //DEMOFFMPEG_NATIVEMEDIACONSTANT_H

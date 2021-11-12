//
// Created by Admin on 2021/7/16.
//

#ifndef DEMOFFMPEG_AUDIOCONFIGURATION_H
#define DEMOFFMPEG_AUDIOCONFIGURATION_H

#include <media/NdkMediaCodec.h>

// type 1-音频，2-视频
typedef void (*OutputFmtChangedListener) (int,AMediaFormat*) ;
// type 1-音频，2-视频
typedef void (*OutputDataListener) (int,AMediaCodecBufferInfo*,uint8_t*) ;

#define DEFAULT_AUDIO_FREQUENCY 44100
#define DEFAULT_AUDIO_MAX_BPS 64
#define DEFAULT_AUDIO_MIN_BPS 32
#define DEFAULT_AUDIO_ADTS 0
#define DEFAULT_AUDIO_MIME "audio/mp4a-latm"
//int DEFAULT_AUDIO_ENCODING AudioFormat.ENCODING_PCM_16BIT
//int DEFAULT_AAC_PROFILE MediaCodecInfo.CodecProfileLevel.AACObjectLC
#define DEFAULT_AUDIO_CHANNEL_COUNT 2
#define DEFAULT_AUDIO_FRAME_SIZE_SINGLE 4096
#define  DEFAULT_AEC false;


#endif //DEMOFFMPEG_AUDIOCONFIGURATION_H

//
// Created by Admin on 2020/10/21.
//

#ifndef DEMOC_FFMPEGENCODEAUDIO_H
#define DEMOC_FFMPEGENCODEAUDIO_H

#include <link.h>

/**
 * 编码为单独文件，混合编码
 */
class FFmpegEncodeAudio {

public:
    void init();

private:
    void encodeLoop();

    void configAudioEncodeParams(AVCodecContext *pContext);

    void configFrameParams(AVFrame *pFrame, AVCodecContext *pContext);
};


#endif //DEMOC_FFMPEGENCODEAUDIO_H

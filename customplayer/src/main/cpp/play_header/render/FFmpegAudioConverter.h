//
// Created by Admin on 2021/5/24.
//

#ifndef DEMOFFMPEG_FFMPEGAUDIOCONVERTER_H
#define DEMOFFMPEG_FFMPEGAUDIOCONVERTER_H

extern "C" {
#include <libswresample/swresample.h>
#include <libavutil/opt.h>
}

#include <media/OpenSLESRender.h>
#include "BaseDataCoverter.h"

/***
 * 音频重采样
 */
class FFmpegAudioConverter : public BaseDataCoverter{
public:

    void Init(AVCodecContext* codecContext) ;
    void Destroy() ;
    void covertData(DataConvertInfo * data) ;


private:
    SwrContext *m_SwrContext;
    int m_BufferSize;
    uint8_t *m_AudioOutBuffer;
};

#endif //DEMOFFMPEG_FFMPEGAUDIOCONVERTER_H

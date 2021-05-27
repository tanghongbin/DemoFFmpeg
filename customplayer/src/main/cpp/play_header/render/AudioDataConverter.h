//
// Created by Admin on 2021/5/24.
//

#ifndef DEMOFFMPEG_AUDIODATACONVERTER_H
#define DEMOFFMPEG_AUDIODATACONVERTER_H

extern "C" {
#include <libswresample/swresample.h>
#include <libavutil/opt.h>
}

#include <media/OpenSLESRender.h>
#include "BaseDataCoverter.h"


class AudioDataConverter : public BaseDataCoverter{
public:

    void Init(AVCodecContext* codecContext) ;
    void Destroy() ;
    void covertData(AVFrame* data) ;


private:
    SwrContext *m_SwrContext;
    int m_BufferSize;
    uint8_t *m_AudioOutBuffer;
    OpenSLESRender *openSlesRender;
};

#endif //DEMOFFMPEG_AUDIODATACONVERTER_H

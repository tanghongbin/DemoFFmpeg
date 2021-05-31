//
// Created by Admin on 2021/5/31.
//

#ifndef DEMOFFMPEG_FFMPEGMEDIAMUXER_H
#define DEMOFFMPEG_FFMPEGMEDIAMUXER_H

#include <thread>
#include "AbsMediaMuxer.h"

class FFmpegMediaMuxer : public AbsMediaMuxer{
private:
    std::thread * thread;
    static int StartMuxer(const char * fileName);
public:
    int init(const char * outFileName);
    void Destroy();
    FFmpegMediaMuxer(){
        thread = 0;
    }
};

#endif //DEMOFFMPEG_FFMPEGMEDIAMUXER_H

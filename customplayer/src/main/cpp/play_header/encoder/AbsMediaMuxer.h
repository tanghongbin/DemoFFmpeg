//
// Created by Admin on 2021/5/31.
//

#ifndef DEMOFFMPEG_ABSMEDIAMUXER_H
#define DEMOFFMPEG_ABSMEDIAMUXER_H

class AbsMediaMuxer{
public:
    virtual int init(const char * outFileName) = 0;
    virtual void Destroy(){

    }
};

#endif //DEMOFFMPEG_ABSMEDIAMUXER_H

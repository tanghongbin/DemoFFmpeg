//
// Created by Admin on 2021/5/24.
//

#include <decoder/ImlDecoder.h>


BaseDataCoverter *AudioDecoder::createConverter()  {
    return new AudioDataConverter;
}




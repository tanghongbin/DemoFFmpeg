//
// Created by Admin on 2020/8/25.
//

#ifndef DEMOC_TEXTURESAMPLE_H
#define DEMOC_TEXTURESAMPLE_H


#include <ImageDef.h>
#include "../../../../../../../android_sdk/android_sdk/sdk/ndk-bundle/toolchains/llvm/prebuilt/windows-x86_64/sysroot/usr/include/GLES3/gl3.h"
#include "GLBaseSample.h"

class TextureSample : public GLBaseSample {

public:

    void init();

    void draw();

    void Destroy();

};


#endif //DEMOC_TEXTURESAMPLE_H

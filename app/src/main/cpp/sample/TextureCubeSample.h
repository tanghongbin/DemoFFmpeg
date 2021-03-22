//
// Created by ByteFlow on 2019/7/10.
//

#ifndef NDK_OPENGLES_3_0_TEXTURECUBESAMPLE_H
#define NDK_OPENGLES_3_0_TEXTURECUBESAMPLE_H

#include "GLBaseSample.h"

class TextureCubeSample : public GLBaseSample
{
public:
	TextureCubeSample();

	virtual ~TextureCubeSample();

	virtual void init(const char * vertexStr,const char * fragStr);

	virtual void draw();

	virtual void Destroy();

};


#endif //NDK_OPENGLES_3_0_TEXTURECUBESAMPLE_H

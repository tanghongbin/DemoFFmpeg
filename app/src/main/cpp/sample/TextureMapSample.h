//
// Created by ByteFlow on 2019/7/10.
//

#ifndef NDK_OPENGLES_3_0_TEXTUREMAPSAMPLE_H
#define NDK_OPENGLES_3_0_TEXTUREMAPSAMPLE_H


#include <ImageDef.h>
#include <GLES3/gl3.h>
#include "GLBaseSample.h"

class TextureMapSample : public GLBaseSample
{
public:
	TextureMapSample();

	virtual ~TextureMapSample();

	virtual void init();

	virtual void draw();

	virtual void Destroy();

};


#endif //NDK_OPENGLES_3_0_TEXTUREMAPSAMPLE_H

//
// Created by ByteFlow on 2019/7/10.
//

#ifndef NDK_OPENGLES_3_0_TEXTURECUBESAMPLE_H
#define NDK_OPENGLES_3_0_TEXTURECUBESAMPLE_H

#include <model/shader.h>
#include "GLBaseSample.h"
#include "Model3DSample.h"

class TextureCubeSample : public GLBaseSample
{

private:
	Shader* innershader;
	GLuint textures[4];
	GLuint mVao[2];
	GLuint mVbo[4];
	Model3DSample * sample;

public:
	TextureCubeSample();

	virtual ~TextureCubeSample();

	virtual void init(const char * vertexStr,const char * fragStr);

	virtual void draw();

	virtual void Destroy();

	void UpdateMvp();

    void drawInnerBox();
};


#endif //NDK_OPENGLES_3_0_TEXTURECUBESAMPLE_H

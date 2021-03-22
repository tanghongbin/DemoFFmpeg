//
// Created by ByteFlow on 2019/7/10.
//

#include <CustomGLUtils.h>
#include "TextureCubeSample.h"

TextureCubeSample::TextureCubeSample()
{
	m_TextureId = 0;
}

TextureCubeSample::~TextureCubeSample()
{
	NativeImageUtil::FreeNativeImage(&m_RenderImage);
}

void TextureCubeSample::init(const char * vertexStr,const char * fragStr)
{
	//create RGBA texture
	glGenTextures(1, &m_TextureId);

	// cube test
	GLubyte  cubes[6][3] = {
			255,0,0,
			0,255,0,
			0,0,255,
			255,255,0,
			255,0,255,
			255,255,255
	};
	glBindTexture(GL_TEXTURE_CUBE_MAP,m_TextureId);
	glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X,0,GL_RGB,1,1,0,GL_RGB,GL_UNSIGNED_BYTE,&cubes[0]);
	glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_X,0,GL_RGB,1,1,0,GL_RGB,GL_UNSIGNED_BYTE,&cubes[1]);
	glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_Y,0,GL_RGB,1,1,0,GL_RGB,GL_UNSIGNED_BYTE,&cubes[2]);
	glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_Y,0,GL_RGB,1,1,0,GL_RGB,GL_UNSIGNED_BYTE,&cubes[3]);
	glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_Z,0,GL_RGB,1,1,0,GL_RGB,GL_UNSIGNED_BYTE,&cubes[4]);
	glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_Z,0,GL_RGB,1,1,0,GL_RGB,GL_UNSIGNED_BYTE,&cubes[5]);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	char vShaderStr[] =
			"#version 300 es\n"
			"layout(location = 0) in vec4 a_position;\n"
			"layout(location = 1) in vec3 a_normal;\n"
			"out vec3 v_normal; \n"
			"void main()                              \n"
			"{                         \n"
			"   v_normal = a_normal;            \n"
			"   gl_Position = a_position;\n"
			"}";

	char fShaderStr[] =
			"#version 300 es                              \n"
			"precision mediump float;   \n"
			"in vec3 v_normal;                     \n"
			"layout(location = 0) out vec4 outColor;\n"
			"uniform samplerCube s_texture;\n"
			"void main()                                  \n"
			"{                                            \n"
			"   outColor = texture(s_texture,v_normal);\n"
			"}";

	m_ProgramObj = CreateProgram(vShaderStr, fShaderStr, m_VertexShader, m_FragmentShader);
	if (m_ProgramObj)
	{
		m_SamplerLoc = glGetUniformLocation(m_ProgramObj, "s_TextureMap");
	}
	else
	{
		LOGCATE("TextureCubeSample::Init create program fail");
	}

}

void TextureCubeSample::draw()
{


	if(m_ProgramObj == GL_NONE || m_TextureId == GL_NONE) return;
//	LOGCATE("TextureCubeSample::Draw()");


	glClear(GL_STENCIL_BUFFER_BIT | GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glClearColor(1.0, 1.0, 1.0, 1.0);

	// 左上角起始点，逆时针读取
	GLfloat verticesCoords[] = {
			-1.0f,  0.5f, 0.0f,  // Position 0
			-1.0f, -0.5f, 0.0f,  // Position 1
			1.0f, -0.5f, 0.0f,   // Position 2
			1.0f,  0.5f, 0.0f,   // Position 3
	};

	// 纹理坐标左下角起始点，顺时针
	GLfloat textureCoords[] = {
			0.0f,  0.0f,        // TexCoord 0
			0.0f,  1.0f,        // TexCoord 1
			1.0f,  1.0f,        // TexCoord 2
			1.0f,  0.0f         // TexCoord 3
	};

	GLushort indices[] = { 0, 1, 2, 0, 3, 2 };

	// Use the program object
	glUseProgram (m_ProgramObj);

	// Load the vertex position
	glVertexAttribPointer (0, 3, GL_FLOAT,
							GL_FALSE, 3 * sizeof (GLfloat), verticesCoords);
	// Load the texture coordinate
	glVertexAttribPointer (1, 2, GL_FLOAT,
							GL_FALSE, 2 * sizeof (GLfloat), textureCoords);

	glEnableVertexAttribArray (0);
	glEnableVertexAttribArray (1);

	LOGCATE("TextureCubeSample::OnDrawFrame [w, h]=[%d, %d], format=%d", m_RenderImage.width, m_RenderImage.height, m_RenderImage.format);

	long long startTime = GetSysCurrentTime();
    // Bind the RGBA map
    //upload RGBA image data
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_CUBE_MAP, m_TextureId);
    long long startTimeUpload = GetSysCurrentTime();

//    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, m_RenderImage.width, m_RenderImage.height, 0, GL_RGBA, GL_UNSIGNED_BYTE, m_RenderImage.ppPlane[0]);

    LOGCATE("every frame cost:%lld",GetSysCurrentTime() - startTime);
    LOGCATE("every frame upload cost:%lld",GetSysCurrentTime() - startTimeUpload);


//	LOGCATE("print texture unit:%d",m_TextureId);
	// Set the RGBA map sampler to texture unit to 0
	// 设置统一变量
	glUniform1i(m_SamplerLoc, GL_TEXTURE0);

	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, indices);
    glBindTexture(GL_TEXTURE_CUBE_MAP, GL_NONE);




}


void TextureCubeSample::Destroy()
{
	if (m_ProgramObj)
	{
		glDeleteProgram(m_ProgramObj);
		glDeleteTextures(1, &m_TextureId);
		m_ProgramObj = GL_NONE;
	}

}

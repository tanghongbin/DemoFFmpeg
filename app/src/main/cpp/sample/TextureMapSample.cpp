//
// Created by ByteFlow on 2019/7/10.
//

#include <CustomGLUtils.h>
#include "TextureMapSample.h"

TextureMapSample::TextureMapSample()
{
	m_TextureId = 0;

}

TextureMapSample::~TextureMapSample()
{
	NativeImageUtil::FreeNativeImage(&m_RenderImage);
}

void TextureMapSample::init(const char * vertexStr,const char * fragStr)
{
	//create RGBA texture
	glGenTextures(1, &m_TextureId);
	glBindTexture(GL_TEXTURE_2D, m_TextureId);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
//	 必须设置的两个属性
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	char vShaderStr[] =
			"#version 300 es                            \n"
			"layout(location = 0) in vec4 a_position;   \n"
			"layout(location = 1) in vec2 a_texCoord;   \n"
			"layout(location = 2) in float inFactor;\n"
			"out float outFactor;\n"
			"out vec2 v_texCoord;                       \n"
			"void main()                                \n"
			"{                   \n"
			"   gl_Position = a_position;               \n"
			"   v_texCoord = a_texCoord;   \n"
			"   outFactor = inFactor;\n"
			"}";

	char fShaderStr[] =
			"#version 300 es                                     \n"
			"precision mediump float;                            \n"
			"in vec2 v_texCoord;\n"
			"in float outFactor;\n"
			"layout(location = 0) out vec4 outColor;             \n"
			"uniform sampler2D s_TextureMap;                     \n"
			"void main()                                         \n"
			"{\n"
			"  outColor = texture(s_TextureMap, v_texCoord);\n"
//			"  outColor = vec4(0.0f,1.0f,0.0f,1.0f);\n"
			"}";

	m_ProgramObj = CreateProgram(vShaderStr, fShaderStr, m_VertexShader, m_FragmentShader);
	if (m_ProgramObj)
	{
		m_SamplerLoc = glGetUniformLocation(m_ProgramObj, "s_TextureMap");
	}
	else
	{
		LOGCATE("TextureMapSample::Init create program fail");
	}

}

void TextureMapSample::draw()
{


	if(m_ProgramObj == GL_NONE || m_TextureId == GL_NONE) return;
//	LOGCATE("TextureMapSample::Draw()");


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
	float value = ((getRandomInt(100)) * 1.0f/ 100.f);
	LOGCATE("log value:%f",value);
	GLfloat facotrs[] = {
			value
	};

//	GLushort indices[] = { 0, 1, 2, 0, 3, 2 };

	// Use the program object
	glUseProgram (m_ProgramObj);

	glEnableVertexAttribArray (0);
	glEnableVertexAttribArray (1);

	// Load the vertex position
	glVertexAttribPointer (0, 3, GL_FLOAT,
							GL_FALSE, 3 * sizeof (GLfloat), verticesCoords);
	// Load the texture coordinate
	glVertexAttribPointer (1, 2, GL_FLOAT,
							GL_FALSE, 2 * sizeof (GLfloat), textureCoords);
//	glVertexAttribPointer (2, 1, GL_FLOAT,
//						   GL_FALSE, 1 * sizeof (GLfloat), facotrs);


//	glEnableVertexAttribArray (2);


	LOGCATE("TextureMapSample::OnDrawFrame [w, h]=[%d, %d], format=%d", m_RenderImage.width, m_RenderImage.height, m_RenderImage.format);

	long long startTime = GetSysCurrentTime();
    // Bind the RGBA map
    //upload RGBA image data
    glBindTexture(GL_TEXTURE_2D, m_TextureId);
    long long startTimeUpload = GetSysCurrentTime();

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, m_RenderImage.width, m_RenderImage.height, 0, GL_RGBA, GL_UNSIGNED_BYTE, m_RenderImage.ppPlane[0]);

    LOGCATE("every frame cost:%lld",GetSysCurrentTime() - startTime);
    LOGCATE("every frame upload cost:%lld",GetSysCurrentTime() - startTimeUpload);


//	LOGCATE("print texture unit:%d",m_TextureId);
	// Set the RGBA map sampler to texture unit to 0
	// 设置统一变量
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, m_TextureId);
	glUniform1i(m_SamplerLoc, 0);
	GLint indics[6] = {0,1,2,0,2,3};

	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT,indics);
    glBindTexture(GL_TEXTURE_2D, GL_NONE);

}


void TextureMapSample::Destroy()
{
	if (m_ProgramObj)
	{
		glDeleteProgram(m_ProgramObj);
		glDeleteTextures(1, &m_TextureId);
		m_ProgramObj = GL_NONE;
	}

}

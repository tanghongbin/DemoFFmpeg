//
// Created by ByteFlow on 2019/7/10.
//

#include <CustomGLUtils.h>
#include "TestFBOSample.h"

TestFBOSample::TestFBOSample()
{
	m_TextureId = 0;

}

TestFBOSample::~TestFBOSample()
{
	NativeImageUtil::FreeNativeImage(&m_RenderImage);
}

void TestFBOSample::init(const char * vertexStr,const char * fragmentStr)
{
	const char * vShaderStr = vertexStr;

	const char * fShaderStr = fragmentStr;

	LOGCATE("顶点字符串:%s",vertexStr);

    LOGCATE("片段字符串:%s",fragmentStr);


    m_ProgramObj = CreateProgram(vShaderStr, fShaderStr, m_VertexShader, m_FragmentShader);
	if (m_ProgramObj)
	{
		m_SamplerLoc = glGetUniformLocation(m_ProgramObj, "s_TextureMap");
	}
	else
	{
		LOGCATE("TestFBOSample::Init create program fail");
	}

    //创建原图像纹理
    glGenTextures(1, &m_TextureId);
    glBindTexture(GL_TEXTURE_2D, m_TextureId);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
//	 必须设置的两个属性
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, m_RenderImage.width, m_RenderImage.height, 0, GL_RGB, GL_UNSIGNED_BYTE, m_RenderImage.ppPlane[0]);
	glBindTexture(GL_TEXTURE_2D,0);

	createFrameBuffer();


}

void TestFBOSample::createFrameBuffer() {// 创建离屏纹理
	glGenTextures(1,&m_fboTextureId);
	glBindTexture(GL_TEXTURE_2D, m_fboTextureId);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
//	 必须设置的两个属性
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, m_RenderImage.width, m_RenderImage.height, 0, GL_RGB, GL_UNSIGNED_BYTE, nullptr);
	glBindTexture(GL_TEXTURE_2D, 0);

	glCheckError("create frame buffer");
	// 创建帧缓冲
	glGenFramebuffers(1,&m_fboId);
	glBindFramebuffer(GL_FRAMEBUFFER, m_fboId);
	glBindTexture(GL_TEXTURE_2D, m_fboTextureId);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_fboTextureId, 0);
	GLenum resultFramebuffer = glCheckFramebufferStatus(GL_FRAMEBUFFER);
	if (resultFramebuffer != GL_FRAMEBUFFER_COMPLETE){
        LOGCATE("frame buffer is not complete 0x%x",resultFramebuffer);
    }
	glBindTexture(GL_TEXTURE_2D, 0);
	glBindFramebuffer(GL_FRAMEBUFFER,0);



//	glGenTextures(1, &m_fboTextureId);
//	glBindTexture(GL_TEXTURE_2D, m_fboTextureId);
//	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
//	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
//	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
//	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
//	glBindTexture(GL_TEXTURE_2D, GL_NONE);
//
//	// 创建并初始化 FBO
//	glGenFramebuffers(1, &m_fboId);
//	glBindFramebuffer(GL_FRAMEBUFFER, m_fboId);
//	glBindTexture(GL_TEXTURE_2D, m_fboTextureId);
//	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_fboTextureId, 0);
//	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, m_RenderImage.width, m_RenderImage.height, 0, GL_UNSIGNED_BYTE, GL_RGB, nullptr);
//	if (glCheckFramebufferStatus(GL_FRAMEBUFFER)!= GL_FRAMEBUFFER_COMPLETE) {
//		LOGCATE("FBOSample::CreateFrameBufferObj glCheckFramebufferStatus status != GL_FRAMEBUFFER_COMPLETE");
//	}
//	glBindTexture(GL_TEXTURE_2D, GL_NONE);
//	glBindFramebuffer(GL_FRAMEBUFFER, GL_NONE);
}

void TestFBOSample::draw()
{


	if(m_ProgramObj == GL_NONE || m_TextureId == GL_NONE) return;
//	LOGCATE("TestFBOSample::Draw()");

	// 左上角起始点，逆时针读取
	GLfloat verticesCoords[] = {
			-1.0f,  1.0f, 0.0f,  // Position 0
			1.0f, 1.0f, 0.0f,  // Position 1
			1.0f, -1.0f, 0.0f,   // Position 2
			-1.0f,  -1.0f, 0.0f,   // Position 3
	};

	// 纹理坐标左下角起始点，顺时针
	GLfloat textureCoords[] = {
			0.0f,  0.0f,        // TexCoord 0
			1.0f,  0.0f,        // TexCoord 1
			1.0f,  1.0f,        // TexCoord 2
			1.0f,  0.0f         // TexCoord 3
	};
	GLfloat textureCoordsWrite[] = {
            0.0f,  1.0f,        // TexCoord 0
            1.0f,  1.0f,        // TexCoord 1
            1.0f,  0.0f,        // TexCoord 2
           0.0f,  0.0f         // TexCoord 3
	};
	float value = ((getRandomInt(100)) * 1.0f/ 100.f);
//	LOGCATE("log value:%f",value);
	GLfloat facotrs[] = {
			value
	};

	GLushort indices[] = { 0, 1, 2, 0, 3, 2 };

	// Use the program object

    glUseProgram(m_ProgramObj);
//    stbi_load("")

	// 离屏渲染
	glBindFramebuffer(GL_FRAMEBUFFER,m_fboId);
    glPixelStorei(GL_UNPACK_ALIGNMENT,1);
    glViewport(0, 0,screenWidth, screenHeight);
    glClear(GL_STENCIL_BUFFER_BIT | GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glClearColor(1.0, 1.0, 1.0, 1.0);

	// Load the vertex position
	glEnableVertexAttribArray (0);
	glEnableVertexAttribArray (1);
	glVertexAttribPointer (0, 3, GL_FLOAT,
						   GL_FALSE, 3 * sizeof (GLfloat), verticesCoords);
	// Load the texture coordinate
	glVertexAttribPointer (1, 2, GL_FLOAT,
						   GL_FALSE, 2 * sizeof (GLfloat), textureCoordsWrite);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, m_TextureId);
	glUniform1i(m_SamplerLoc, 0);
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, indices);
	glBindTexture(GL_TEXTURE_2D, 0);
	glBindFramebuffer(GL_FRAMEBUFFER,0);


	glViewport(0, 0,screenWidth, screenHeight);

    glClear(GL_STENCIL_BUFFER_BIT | GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glClearColor(1.0, 1.0, 1.0, 1.0);

	// 普通渲染

	// Load the vertex position
	glVertexAttribPointer (0, 3, GL_FLOAT,
							GL_FALSE, 3 * sizeof (GLfloat), verticesCoords);
	// Load the texture coordinate
	glVertexAttribPointer (1, 2, GL_FLOAT,
							GL_FALSE, 2 * sizeof (GLfloat), textureCoords);

	glEnableVertexAttribArray (0);
	glEnableVertexAttribArray (1);

	// 设置统一变量
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, m_fboTextureId);
	glUniform1i(m_SamplerLoc, 0);

	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, indices);
    glBindTexture(GL_TEXTURE_2D, GL_NONE);

}


void TestFBOSample::Destroy()
{
	if (m_ProgramObj)
	{
		glDeleteProgram(m_ProgramObj);
		glDeleteTextures(1, &m_TextureId);
		m_ProgramObj = GL_NONE;
	}

}

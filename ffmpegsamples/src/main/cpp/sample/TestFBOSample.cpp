//
// Created by ByteFlow on 2019/7/10.
//

#include <CustomGLUtils.h>
#include "TestFBOSample.h"
#include "stb_image.h"

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

//	LOGCATE("顶点字符串:%s",vertexStr);

//    LOGCATE("片段字符串:%s",fragmentStr);


    m_ProgramObj = CreateProgram(vShaderStr, fShaderStr, m_VertexShader, m_FragmentShader);
	if (m_ProgramObj)
	{
		m_SamplerLoc = glGetUniformLocation(m_ProgramObj, "s_TextureMap");
	}
	else
	{
		LOGCATE("TestFBOSample::Init create program fail");
	}

	int localWidth;
	int localHeight;
	int nrChannels;
	const char * filePath = "/storage/emulated/0/ffmpegtest/filterImg/test1.jpg";
	const char * filePath2 = "/storage/emulated/0/ffmpegtest/filterImg/test2.png";
    imageData = stbi_load(filePath2,&localWidth,&localHeight,&nrChannels,0);
    LOGCATE("image width:%d height:%d channel:%d data:%p",localWidth,localHeight,nrChannels,imageData);
    //创建原图像纹理
    glGenTextures(1, &m_TextureId);
    glBindTexture(GL_TEXTURE_2D, m_TextureId);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
//	 必须设置的两个属性
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
//	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, localWidth, localHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, imageData);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, m_RenderImage.width, m_RenderImage.height, 0, GL_RGBA, GL_UNSIGNED_BYTE,m_RenderImage.ppPlane[0]);
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
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, m_RenderImage.width, m_RenderImage.height, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
	glBindTexture(GL_TEXTURE_2D, 0);

	glCheckError("create frame buffer");
	// 创建帧缓冲
	glGenFramebuffers(1,&m_fboId);
	glBindFramebuffer(GL_FRAMEBUFFER, m_fboId);
	glBindTexture(GL_TEXTURE_2D, m_TextureId);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_fboTextureId, 0);
	GLenum resultFramebuffer = glCheckFramebufferStatus(GL_FRAMEBUFFER);
	if (resultFramebuffer != GL_FRAMEBUFFER_COMPLETE){
        LOGCATE("frame buffer is not complete 0x%x",resultFramebuffer);
    }
	glBindTexture(GL_TEXTURE_2D, 0);
	glBindFramebuffer(GL_FRAMEBUFFER,0);


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
            0.0f,  1.0f         // TexCoord 3
    };
    GLfloat textureCoordsWrite[] = {
            0.0f,  1.0f,        // TexCoord 0
            1.0f,  1.0f,        // TexCoord 1
            1.0f,  0.0f,        // TexCoord 2
            0.0f,  0.0f         // TexCoord 3
    };

	glGenBuffers(3,vboIds);
	glBindBuffer(GL_ARRAY_BUFFER,vboIds[0]);
	glBufferData(GL_ARRAY_BUFFER,sizeof(verticesCoords),verticesCoords,GL_STATIC_DRAW);

    glBindBuffer(GL_ARRAY_BUFFER,vboIds[1]);
    glBufferData(GL_ARRAY_BUFFER,sizeof(textureCoords),textureCoords,GL_STATIC_DRAW);

    glBindBuffer(GL_ARRAY_BUFFER,vboIds[2]);
    glBufferData(GL_ARRAY_BUFFER,sizeof(textureCoordsWrite),textureCoordsWrite,GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER,0);


}

void TestFBOSample::draw()
{


	if(m_ProgramObj == GL_NONE || m_TextureId == GL_NONE) return;
//	LOGCATE("TestFBOSample::Draw()");

    glPixelStorei(GL_UNPACK_ALIGNMENT,1);
    glViewport(0, 0, screenWidth, screenHeight);

	GLushort indices[] = { 0, 1, 2, 0, 2, 3 };

	// Use the program object

    glUseProgram(m_ProgramObj);
//    stbi_load("")

	// 离屏渲染
	glBindFramebuffer(GL_FRAMEBUFFER,m_fboId);
    glClear(GL_STENCIL_BUFFER_BIT | GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glClearColor(1.0, 1.0, 1.0, 1.0);
	// Load the vertex position
	glEnableVertexAttribArray (0);
	glEnableVertexAttribArray (1);
	glBindBuffer(GL_ARRAY_BUFFER,vboIds[0]);
	glVertexAttribPointer (0, 3, GL_FLOAT,
						   GL_FALSE, 0, (const void *)0);
	// Load the texture coordinate
    glBindBuffer(GL_ARRAY_BUFFER,vboIds[2]);
	glVertexAttribPointer (1, 2, GL_FLOAT,
						   GL_FALSE, 0, (const void *)0);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, m_TextureId);
	glUniform1i(m_SamplerLoc, 0);
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, indices);
	glBindTexture(GL_TEXTURE_2D, 0);
	glBindFramebuffer(GL_FRAMEBUFFER,0);



    glClear(GL_STENCIL_BUFFER_BIT | GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glClearColor(1.0, 1.0, 1.0, 1.0);
	// 普通渲染
    glBindBuffer(GL_ARRAY_BUFFER,vboIds[0]);
	// Load the vertex position
	glVertexAttribPointer (0, 3, GL_FLOAT,
							GL_FALSE,  0, (const void *)0);
	// Load the texture coordinate
    glBindBuffer(GL_ARRAY_BUFFER,vboIds[1]);
	glVertexAttribPointer (1, 2, GL_FLOAT,
							GL_FALSE, 0, (const void *)0);

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
	    if (imageData) {
            stbi_image_free(imageData);
            imageData = nullptr;
	    }
		glDeleteProgram(m_ProgramObj);
		glDeleteTextures(1, &m_TextureId);
		m_ProgramObj = GL_NONE;
		LOGCATE("ondestroy over");
	}

}

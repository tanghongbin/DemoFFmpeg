//
// Created by ByteFlow on 2019/7/10.
//

#include <CustomGLUtils.h>
#include "TextureCubeSample.h"
#include "Model3DSample.h"
#include <vector>
#include <camera.h>

TextureCubeSample::TextureCubeSample()
{
	m_TextureId = 0;
}

TextureCubeSample::~TextureCubeSample()
{
	NativeImageUtil::FreeNativeImage(&m_RenderImage);
}

void TextureCubeSample::init(const char * vShaderStr,const char * fShaderStr)
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
	std::vector<std::string> faces
			{
					"right.jpg",
					"left.jpg",
					"top.jpg",
					"bottom.jpg",
					"front.jpg",
					"back.jpg"
			};
	for (int i = 0; i < faces.size(); ++i) {
		LoadImageInfo loadImageInfo;
		std::string fileName = std::string(FILTER_IMAGE_DIR) + "skybox/" + faces[i];
		loadImageInfo.loadImage(fileName.c_str());
		loadImageInfo.uploadCubeTex(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i);
	}
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	glBindTexture(GL_TEXTURE_CUBE_MAP,0);

	glGenTextures(4,textures);
	glBindTexture(GL_TEXTURE_2D, textures[0]);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    LoadImageInfo loadImageInfo;
    loadImageInfo.loadImage("/storage/emulated/0/ffmpegtest/filterImg/marble.jpg");
    loadImageInfo.uploadImageTex2D();
    glBindTexture(GL_TEXTURE_2D,0);

	m_ProgramObj = CreateProgram(vShaderStr, fShaderStr, m_VertexShader, m_FragmentShader);
	if (m_ProgramObj)
	{
		m_SamplerLoc = glGetUniformLocation(m_ProgramObj, "s_TextureMap");
	}
	else
	{
		LOGCATE("TextureCubeSample::Init create program fail");
	}
	m_ScaleX = 3.17f;
	m_ScaleY = 3.17f;

	std::string ver = readGLSLStrFromFile("cube/innervetex.glsl");
	std::string fragment = readGLSLStrFromFile("cube/innerfragment.glsl");
	innershader = new Shader(ver.c_str(),fragment.c_str());

	// bind vao,vbo
	float verticesInner[] = {
			// position,texcoords,normal
			-0.5f, -0.5f, -0.5f,  0.0f, 0.0f, 0.0f,  0.0f, -1.0f,
			0.5f, -0.5f, -0.5f,  1.0f, 0.0f,  0.0f,  0.0f, -1.0f,
			0.5f,  0.5f, -0.5f,  1.0f, 1.0f,  0.0f,  0.0f, -1.0f,
			0.5f,  0.5f, -0.5f,  1.0f, 1.0f,  0.0f,  0.0f, -1.0f,
			-0.5f,  0.5f, -0.5f,  0.0f, 1.0f,  0.0f,  0.0f, -1.0f,
			-0.5f, -0.5f, -0.5f,  0.0f, 0.0f,  0.0f,  0.0f, -1.0f,

			-0.5f, -0.5f,  0.5f,  0.0f, 0.0f,   0.0f,  0.0f,  1.0f,
			0.5f, -0.5f,  0.5f,  1.0f, 0.0f,   0.0f,  0.0f,  1.0f,
			0.5f,  0.5f,  0.5f,  1.0f, 1.0f,   0.0f,  0.0f,  1.0f,
			0.5f,  0.5f,  0.5f,  1.0f, 1.0f,   0.0f,  0.0f,  1.0f,
			-0.5f,  0.5f,  0.5f,  0.0f, 1.0f,   0.0f,  0.0f,  1.0f,
			-0.5f, -0.5f,  0.5f,  0.0f, 0.0f,   0.0f,  0.0f,  1.0f,

			-0.5f,  0.5f,  0.5f,  1.0f, 0.0f,  1.0f,  0.0f,  0.0f,
			-0.5f,  0.5f, -0.5f,  1.0f, 1.0f,  1.0f,  0.0f,  0.0f,
			-0.5f, -0.5f, -0.5f,  0.0f, 1.0f,  1.0f,  0.0f,  0.0f,
			-0.5f, -0.5f, -0.5f,  0.0f, 1.0f,  1.0f,  0.0f,  0.0f,
			-0.5f, -0.5f,  0.5f,  0.0f, 0.0f,  1.0f,  0.0f,  0.0f,
			-0.5f,  0.5f,  0.5f,  1.0f, 0.0f,  1.0f,  0.0f,  0.0f,

			0.5f,  0.5f,  0.5f,  1.0f, 0.0f,  1.0f,  0.0f,  0.0f,
			0.5f,  0.5f, -0.5f,  1.0f, 1.0f,  1.0f,  0.0f,  0.0f,
			0.5f, -0.5f, -0.5f,  0.0f, 1.0f,  1.0f,  0.0f,  0.0f,
			0.5f, -0.5f, -0.5f,  0.0f, 1.0f,  1.0f,  0.0f,  0.0f,
			0.5f, -0.5f,  0.5f,  0.0f, 0.0f,  1.0f,  0.0f,  0.0f,
			0.5f,  0.5f,  0.5f,  1.0f, 0.0f,  1.0f,  0.0f,  0.0f,

			-0.5f, -0.5f, -0.5f,  0.0f, 1.0f,  0.0f, -1.0f,  0.0f,
			0.5f, -0.5f, -0.5f,  1.0f, 1.0f,  0.0f, -1.0f,  0.0f,
			0.5f, -0.5f,  0.5f,  1.0f, 0.0f,  0.0f, -1.0f,  0.0f,
			0.5f, -0.5f,  0.5f,  1.0f, 0.0f,  0.0f, -1.0f,  0.0f,
			-0.5f, -0.5f,  0.5f,  0.0f, 0.0f,  0.0f, -1.0f,  0.0f,
			-0.5f, -0.5f, -0.5f,  0.0f, 1.0f,  0.0f, -1.0f,  0.0f,

			-0.5f,  0.5f, -0.5f,  0.0f, 1.0f,   0.0f,  1.0f,  0.0f,
			0.5f,  0.5f, -0.5f,  1.0f, 1.0f,   0.0f,  1.0f,  0.0f,
			0.5f,  0.5f,  0.5f,  1.0f, 0.0f,   0.0f,  1.0f,  0.0f,
			0.5f,  0.5f,  0.5f,  1.0f, 0.0f,   0.0f,  1.0f,  0.0f,
			-0.5f,  0.5f,  0.5f,  0.0f, 0.0f,   0.0f,  1.0f,  0.0f,
			-0.5f,  0.5f, -0.5f,  0.0f, 1.0f,   0.0f,  1.0f,  0.0f
	};

    glGenBuffers(2,mVao);
    glGenBuffers(4,mVbo);
    glBindBuffer(GL_ARRAY_BUFFER,mVbo[0]);
    glBufferData(GL_ARRAY_BUFFER,sizeof(verticesInner),verticesInner,GL_STATIC_DRAW);
    glBindVertexArray(mVao[0]);
    glBindBuffer(GL_ARRAY_BUFFER,mVbo[0]);
	glVertexAttribPointer (0, 3, GL_FLOAT,
						   GL_FALSE, 8 * sizeof(GLfloat), (void *)0);
	glEnableVertexAttribArray (0);

	glVertexAttribPointer (1, 3, GL_FLOAT,
						   GL_FALSE, 8 * sizeof(GLfloat), (void *)(5 * sizeof(GLfloat)));
	glEnableVertexAttribArray (1);
    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER,0);

	// 左上角起始点，逆时针读取
	float skyboxVertices[] = {
			// positions
			-1.0f,  1.0f, -1.0f,
			-1.0f, -1.0f, -1.0f,
			1.0f, -1.0f, -1.0f,
			1.0f, -1.0f, -1.0f,
			1.0f,  1.0f, -1.0f,
			-1.0f,  1.0f, -1.0f,

			-1.0f, -1.0f,  1.0f,
			-1.0f, -1.0f, -1.0f,
			-1.0f,  1.0f, -1.0f,
			-1.0f,  1.0f, -1.0f,
			-1.0f,  1.0f,  1.0f,
			-1.0f, -1.0f,  1.0f,

			1.0f, -1.0f, -1.0f,
			1.0f, -1.0f,  1.0f,
			1.0f,  1.0f,  1.0f,
			1.0f,  1.0f,  1.0f,
			1.0f,  1.0f, -1.0f,
			1.0f, -1.0f, -1.0f,

			-1.0f, -1.0f,  1.0f,
			-1.0f,  1.0f,  1.0f,
			1.0f,  1.0f,  1.0f,
			1.0f,  1.0f,  1.0f,
			1.0f, -1.0f,  1.0f,
			-1.0f, -1.0f,  1.0f,

			-1.0f,  1.0f, -1.0f,
			1.0f,  1.0f, -1.0f,
			1.0f,  1.0f,  1.0f,
			1.0f,  1.0f,  1.0f,
			-1.0f,  1.0f,  1.0f,
			-1.0f,  1.0f, -1.0f,

			-1.0f, -1.0f, -1.0f,
			-1.0f, -1.0f,  1.0f,
			1.0f, -1.0f, -1.0f,
			1.0f, -1.0f, -1.0f,
			-1.0f, -1.0f,  1.0f,
			1.0f, -1.0f,  1.0f
	};
	glBindBuffer(GL_ARRAY_BUFFER,mVbo[1]);
	glBufferData(GL_ARRAY_BUFFER,sizeof(skyboxVertices),skyboxVertices,GL_STATIC_DRAW);
    glBindVertexArray(mVao[1]);
	glBindBuffer(GL_ARRAY_BUFFER,mVbo[1]);
	glVertexAttribPointer (0, 3, GL_FLOAT,
						   GL_FALSE, 3 * sizeof(GLfloat), (void *)0);
	glEnableVertexAttribArray (0);
	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER,0);

    // 引入模型加载

	std::string modelVetex = readGLSLStrFromFile("model3d/vetex.glsl");
	std::string modelFragment = readGLSLStrFromFile("model3d/fragment.glsl");
	sample = new Model3DSample;
	sample->ifNeedClear = false;
	sample->init(modelVetex.c_str(),modelFragment.c_str());
	sample->screenWidth = screenWidth;
	sample->screenHeight = screenHeight;
}

void TextureCubeSample::draw()
{


	if(m_ProgramObj == GL_NONE || m_TextureId == GL_NONE) return;
	LOGCATE("TextureCubeSample::Draw()");


	glClear(GL_STENCIL_BUFFER_BIT | GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glClearColor(1.0, 1.0, 1.0, 1.0);
	glEnable(GL_DEPTH_TEST);
//	glDepthFunc(GL_GREATER);
	glDepthFunc(GL_LEQUAL);


//	drawInnerBox();

//	 Use the program object
	glUseProgram (m_ProgramObj);
//    m_ScaleX = 4.17f;
//    m_ScaleY = 4.17f;
	UpdateMvp();
	// Load the vertex position
	glBindVertexArray(mVao[1]);
	setMat4(m_ProgramObj,"u_MVPMatrix",mBaseMvpMatrix);
	glActiveTexture(GL_TEXTURE13);
    glBindTexture(GL_TEXTURE_CUBE_MAP, m_TextureId);
	setInt(m_ProgramObj,"cubeMap",13);

	// Set the RGBA map sampler to texture unit to 0
	// 设置统一变量
	glDrawArrays(GL_TRIANGLES, 0, 36);

	// 画模型
//	m_ScaleX = 1.5f;
//	m_ScaleY = 1.5f;
	sample->UpdateTransformMatrix(m_AngleX,m_AngleY,m_ScaleX,m_ScaleY);
	sample->draw();

}

void TextureCubeSample::drawInnerBox() {// 画小三角形
	innershader->use();
	m_ScaleX = 0.3f;
	m_ScaleY = 0.3f;
	UpdateMvp();
	glBindVertexArray(mVao[0]);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_CUBE_MAP, m_TextureId);
	innershader->setInt("cubeMap", 0);
	innershader->setMat4("u_MVPMatrix", mBaseMvpMatrix);
	innershader->setMat4("u_Model",mBaseModel);
	innershader->setVec3("cameraPos",0.0f,0.0f,3.0f);
	glDrawArrays(GL_TRIANGLES, 0, 36);
	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER,0);
}


void TextureCubeSample::UpdateMvp(){
	float angleX = m_AngleX % 360;
	float angleY = m_AngleY % 360;

	//转化为弧度角
	float radiansX = static_cast<float>(MATH_PI / 180.0f * angleX);
	float radiansY = static_cast<float>(MATH_PI / 180.0f * angleY);


	// Projection matrix
//glm::mat4 Projection = glm::ortho(-ratio, ratio, -1.0f, 1.0f, 0.0f, 100.0f);
//glm::mat4 Projection = glm::frustum(-ratio, ratio, -1.0f, 1.0f, 4.0f, 100.0f);
	glm::mat4 Projection = glm::perspective(45.0f, (float )screenWidth/(float )screenHeight, 0.1f, 100.f);

	// View matrix
//	glm::mat4 View = glm::lookAt(
//			glm::vec3(0, 0, 1.8), // Camera is at (0,0,1), in World Space
//			glm::vec3(0, 0, -1), // and looks at the origin
//			glm::vec3(0, 1, 0)  // Head is up (set to 0,-1,0 to look upside-down)
//	);

	glm::mat4 View = glm::lookAt(
			glm::vec3(0, 0, 3), // Camera is at (0,0,1), in World Space
			glm::vec3(0, 0, 0), // and looks at the origin
			glm::vec3(0, 1, 0)  // Head is up (set to 0,-1,0 to look upside-down)
	);

	// Model matrix
	glm::mat4 Model = glm::mat4(1.0f);
	Model = glm::scale(Model, glm::vec3(m_ScaleX, m_ScaleX, m_ScaleX));
	Model = glm::rotate(Model, radiansX, glm::vec3(1.0f, 0.0f, 0.0f));
	Model = glm::rotate(Model, radiansY, glm::vec3(0.0f, 1.0f, 0.0f));
	Model = glm::translate(Model, glm::vec3(0.0f, 0.0f, 0.0f));

	mBaseModel = Model;
	mBaseView = View;
	mBaseProjection = Projection;

	mBaseMvpMatrix = Projection * View * Model;
}

void TextureCubeSample::Destroy()
{
	if (m_ProgramObj)
	{
		sample->Destroy();
		delete sample;
		sample = 0;
		innershader->Destroy();
		innershader = 0;
		glDeleteProgram(m_ProgramObj);
		glDeleteTextures(1, &m_TextureId);
		glDeleteTextures(4, textures);
		glDeleteBuffers(2,mVao);
		glDeleteBuffers(4,mVbo);
		m_ProgramObj = GL_NONE;
	}

}

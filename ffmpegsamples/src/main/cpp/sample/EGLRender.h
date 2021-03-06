//
// Created by ByteFlow on 2019/7/17.
//

#ifndef NDK_OPENGLES_3_0_BGRENDER_H
#define NDK_OPENGLES_3_0_BGRENDER_H
#include "stdint.h"
#include <GLES3/gl3.h>
#include <ImageDef.h>
#include <EGL/egl.h>
#include <EGL/eglext.h>

#define EGL_FEATURE_NUM 8

class EGLRender
{
public:
	EGLRender();
	~EGLRender();

    void Init(int type);

    int CreateGlesEnv();

    //void CreateProgramWithFeedback(const char *vShaderStr, const char *fShaderStr);

    void SetImageData(uint8_t *pData, int width, int height);

    void SetIntParams(int paramType, int param);

    void Draw();

    void DestroyGlesEnv();

    void UnInit();

    static EGLRender* GetInstance()
	{
		if (m_Instance == nullptr)
		{
			m_Instance = new EGLRender();
		}

		return m_Instance;
	}

    static void DestroyInstance()
	{
		if (m_Instance)
		{
			delete m_Instance;
			m_Instance = nullptr;
		}

	}

	void GO_CHECK_GL_ERROR();

private:
	static EGLRender *m_Instance;
	GLuint m_ImageTextureId;
	GLuint m_FboTextureId;
	GLuint m_FboId;
	GLuint m_VaoIds[1];
	GLuint m_VboIds[3];
	GLint m_SamplerLoc;
	GLint m_TexSizeLoc;
	NativeImage m_RenderImage;
	GLuint m_ProgramObj;
	GLuint m_VertexShader;
	GLuint m_FragmentShader;

	EGLConfig  m_eglConf;
	EGLSurface m_eglSurface;
	EGLContext m_eglCtx;
	EGLDisplay m_eglDisplay;
	bool       m_IsGLContextReady;
	const char*m_fShaderStrs[EGL_FEATURE_NUM];
	int        m_ShaderIndex;
};


#endif //NDK_OPENGLES_3_0_BGRENDER_H

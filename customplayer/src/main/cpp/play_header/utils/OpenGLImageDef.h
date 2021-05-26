//
// Created by ByteFlow on 2019/7/10.
//

#ifndef OPEN_GL_IMAGE_DEF
#define OPEN_GL_IMAGE_DEF

#include <malloc.h>
#include <string.h>
#include <unistd.h>
#include "stdio.h"
#include "sys/stat.h"
#include "stdint.h"
#include <GLES3/gl3.h>
#include "stb_image.h"
#include <time.h>
#include <glm/gtc/matrix_transform.hpp>

#define IMAGE_FORMAT_RGBA           0x01
#define IMAGE_FORMAT_NV21           0x02
#define IMAGE_FORMAT_NV12           0x03
#define IMAGE_FORMAT_I420           0x04

#define IMAGE_FORMAT_RGBA_EXT       "RGB32"
#define IMAGE_FORMAT_NV21_EXT       "NV21"
#define IMAGE_FORMAT_NV12_EXT       "NV12"
#define IMAGE_FORMAT_I420_EXT       "I420"

typedef struct _tag_LoadImageInfo_{
private:
	int width;
	int height;
	int channels;
	int64_t startTime;
	stbi_uc * imageData = nullptr;
	int64_t getTime() {
		struct timeval time;
		gettimeofday(&time, NULL);
		long long curTime = ((long long) (time.tv_sec)) * 1000 + time.tv_usec / 1000;
		return curTime;
	}

public:

	~_tag_LoadImageInfo_(){
		LOGCATE("imageData before freed:%p",imageData);
		if (imageData){
			stbi_image_free(imageData);
			imageData = nullptr;
			LOGCATE("imageData has been freed:%p",imageData);
		}
	}
	GLint getFormat(){
		if (channels == 3) {
			return GL_RGB;
		} else {
			return GL_RGBA;
		}
	}
	/**
	 * 上传2D纹理
	 */
	void uploadImageTex2D(){
		glTexImage2D(GL_TEXTURE_2D, 0, getFormat(), width,height, 0, getFormat(), GL_UNSIGNED_BYTE, imageData);
		LOGCATE("load and upload totally cost:%lld",(getTime() - startTime));
	}
	/***
	 * 上传3D纹理
	 */
	void uploadCubeTex(GLenum face){
		glTexImage2D(face, 0, getFormat(), width,height, 0, getFormat(), GL_UNSIGNED_BYTE, imageData);
		LOGCATE("load and upload totally cost:%lld",(getTime() - startTime));
	}
	void loadImage(const char * filePath){
		startTime = getTime();
		imageData = stbi_load(filePath, &width, &height, &channels, 0);
	}

	void setupNormalSetting(){
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	}

} LoadImageInfo;

typedef struct _tag_NativeRectF_plus
{
	float left;
	float top;
	float right;
	float bottom;
	_tag_NativeRectF_plus()
	{
		left = top = right = bottom = 0.0f;
	}
} PlusRectF;

typedef struct _tag_NativeImage_plus
{
	int width;
	int height;
	int format;
	uint8_t *ppPlane[3];
	int pLineSize[3];
	_tag_NativeImage_plus()
	{
		width = 0;
		height = 0;
		format = 0;
		ppPlane[0] = nullptr;
		ppPlane[1] = nullptr;
		ppPlane[2] = nullptr;
		pLineSize[0] = 0;
		pLineSize[1] = 0;
		pLineSize[2] = 0;
	}
} NativeOpenGLImage;

class NativeOpenGLImageUtil
{
public:
	static void AllocNativeImage(NativeOpenGLImage *pImage)
	{
		if (pImage->height == 0 || pImage->width == 0) return;

		switch (pImage->format)
		{
			case IMAGE_FORMAT_RGBA:
			{
				pImage->ppPlane[0] = static_cast<uint8_t *>(malloc(pImage->width * pImage->height * 4));
				pImage->pLineSize[0] = pImage->width * 4;
				pImage->pLineSize[1] = 0;
				pImage->pLineSize[2] = 0;
			}
				break;
			case IMAGE_FORMAT_NV12:
			case IMAGE_FORMAT_NV21:
			{
				pImage->ppPlane[0] = static_cast<uint8_t *>(malloc(pImage->width * pImage->height * 1.5));
				pImage->ppPlane[1] = pImage->ppPlane[0] + pImage->width * pImage->height;
				pImage->pLineSize[0] = pImage->width;
				pImage->pLineSize[1] = pImage->width;
				pImage->pLineSize[2] = 0;
			}
				break;
			case IMAGE_FORMAT_I420:
			{
				pImage->ppPlane[0] = static_cast<uint8_t *>(malloc(pImage->width * pImage->height * 1.5));
				pImage->ppPlane[1] = pImage->ppPlane[0] + pImage->width * pImage->height;
				pImage->ppPlane[2] = pImage->ppPlane[1] + (pImage->width >> 1) * (pImage->height >> 1);
				pImage->pLineSize[0] = pImage->width;
				pImage->pLineSize[1] = pImage->width / 2;
				pImage->pLineSize[2] = pImage->width / 2;
			}
				break;
			default:
//				LOGCATE("NativeImageUtil::AllocNativeImage do not support the format. Format = %d", pImage->format);
				break;
		}
	}

	static void FreeNativeImage(NativeOpenGLImage *pImage)
	{
		if (pImage == nullptr || pImage->ppPlane[0] == nullptr) return;

		free(pImage->ppPlane[0]);
		pImage->ppPlane[0] = nullptr;
		pImage->ppPlane[1] = nullptr;
		pImage->ppPlane[1] = nullptr;
	}

	static void CopyNativeImage(NativeOpenGLImage *pSrcImg, NativeOpenGLImage *pDstImg)
	{
//	    LOGCATE("NativeImageUtil::CopyNativeImage src[w,h,format]=[%d, %d, %d], dst[w,h,format]=[%d, %d, %d]", pSrcImg->width, pSrcImg->height, pSrcImg->format, pDstImg->width, pDstImg->height, pDstImg->format);
//        LOGCATE("NativeImageUtil::CopyNativeImage src[line0,line1,line2]=[%d, %d, %d], dst[line0,line1,line2]=[%d, %d, %d]", pSrcImg->pLineSize[0], pSrcImg->pLineSize[1], pSrcImg->pLineSize[2], pDstImg->pLineSize[0], pDstImg->pLineSize[1], pDstImg->pLineSize[2]);

        if(pSrcImg == nullptr || pSrcImg->ppPlane[0] == nullptr) return;

		if(pSrcImg->format != pDstImg->format ||
		   pSrcImg->width != pDstImg->width ||
		   pSrcImg->height != pDstImg->height)
		{
			LOGCATE("NativeImageUtil::CopyNativeImage invalid params.");
			return;
		}

		if(pDstImg->ppPlane[0] == nullptr) AllocNativeImage(pDstImg);

		switch (pSrcImg->format)
		{
			case IMAGE_FORMAT_I420:
			{
				// y plane
				if(pSrcImg->pLineSize[0] != pDstImg->pLineSize[0]) {
					for (int i = 0; i < pSrcImg->height; ++i) {
						memcpy(pDstImg->ppPlane[0] + i * pDstImg->pLineSize[0], pSrcImg->ppPlane[0] + i * pSrcImg->pLineSize[0], pDstImg->width);
					}
				}
				else
				{
					memcpy(pDstImg->ppPlane[0], pSrcImg->ppPlane[0], pDstImg->pLineSize[0] * pSrcImg->height);
				}

				// u plane
				if(pSrcImg->pLineSize[1] != pDstImg->pLineSize[1]) {
					for (int i = 0; i < pSrcImg->height / 2; ++i) {
						memcpy(pDstImg->ppPlane[1] + i * pDstImg->pLineSize[1], pSrcImg->ppPlane[1] + i * pSrcImg->pLineSize[1], pDstImg->width / 2);
					}
				}
				else
				{
					memcpy(pDstImg->ppPlane[1], pSrcImg->ppPlane[1], pDstImg->pLineSize[1] * pSrcImg->height / 2);
				}

				// v plane
				if(pSrcImg->pLineSize[2] != pDstImg->pLineSize[2]) {
					for (int i = 0; i < pSrcImg->height / 2; ++i) {
						memcpy(pDstImg->ppPlane[2] + i * pDstImg->pLineSize[2], pSrcImg->ppPlane[2] + i * pSrcImg->pLineSize[2], pDstImg->width / 2);
					}
				}
				else
				{
					memcpy(pDstImg->ppPlane[2], pSrcImg->ppPlane[2], pDstImg->pLineSize[2] * pSrcImg->height / 2);
				}
			}
			    break;
			case IMAGE_FORMAT_NV21:
			case IMAGE_FORMAT_NV12:
			{
				// y plane
				if(pSrcImg->pLineSize[0] != pDstImg->pLineSize[0]) {
					for (int i = 0; i < pSrcImg->height; ++i) {
						memcpy(pDstImg->ppPlane[0] + i * pDstImg->pLineSize[0], pSrcImg->ppPlane[0] + i * pSrcImg->pLineSize[0], pDstImg->width);
					}
				}
				else
				{
					memcpy(pDstImg->ppPlane[0], pSrcImg->ppPlane[0], pDstImg->pLineSize[0] * pSrcImg->height);
				}

				// uv plane
				if(pSrcImg->pLineSize[1] != pDstImg->pLineSize[1]) {
					for (int i = 0; i < pSrcImg->height / 2; ++i) {
						memcpy(pDstImg->ppPlane[1] + i * pDstImg->pLineSize[1], pSrcImg->ppPlane[1] + i * pSrcImg->pLineSize[1], pDstImg->width);
					}
				}
				else
				{
					memcpy(pDstImg->ppPlane[1], pSrcImg->ppPlane[1], pDstImg->pLineSize[1] * pSrcImg->height);
				}
			}
				break;
			case IMAGE_FORMAT_RGBA:
			{
				if(pSrcImg->pLineSize[0] != pDstImg->pLineSize[0])
				{
					for (int i = 0; i < pSrcImg->height; ++i) {
						memcpy(pDstImg->ppPlane[0] + i * pDstImg->pLineSize[0], pSrcImg->ppPlane[0] + i * pSrcImg->pLineSize[0], pDstImg->width * 4);
					}
				} else {
					memcpy(pDstImg->ppPlane[0], pSrcImg->ppPlane[0], pSrcImg->pLineSize[0] * pSrcImg->height);
				}
			}
				break;
			default:
			{
				LOGCATE("NativeImageUtil::CopyNativeImage do not support the format. Format = %d", pSrcImg->format);
			}
				break;
		}

	}

	static void DumpNativeImage(NativeOpenGLImage *pSrcImg, const char *pPath, const char *pFileName)
	{
		if (pSrcImg == nullptr || pPath == nullptr || pFileName == nullptr) return;

		if(access(pPath, 0) == -1)
		{
			mkdir(pPath, 0666);
		}

		char imgPath[256] = {0};
		const char *pExt = nullptr;
		switch (pSrcImg->format)
		{
			case IMAGE_FORMAT_I420:
				pExt = IMAGE_FORMAT_I420_EXT;
				break;
			case IMAGE_FORMAT_NV12:
				pExt = IMAGE_FORMAT_NV12_EXT;
				break;
			case IMAGE_FORMAT_NV21:
				pExt = IMAGE_FORMAT_NV21_EXT;
				break;
			case IMAGE_FORMAT_RGBA:
				pExt = IMAGE_FORMAT_RGBA_EXT;
				break;
			default:
				pExt = "Default";
				break;
		}

		sprintf(imgPath, "%s/IMG_%dx%d_%s.%s", pPath, pSrcImg->width, pSrcImg->height, pFileName, pExt);

		FILE *fp = fopen(imgPath, "wb");

//		LOGCATE("DumpNativeImage fp=%p, file=%s", fp, imgPath);

		if(fp)
		{
			switch (pSrcImg->format)
			{
				case IMAGE_FORMAT_I420:
				{
					fwrite(pSrcImg->ppPlane[0],
						   static_cast<size_t>(pSrcImg->width * pSrcImg->height), 1, fp);
					fwrite(pSrcImg->ppPlane[1],
						   static_cast<size_t>((pSrcImg->width >> 1) * (pSrcImg->height >> 1)), 1, fp);
					fwrite(pSrcImg->ppPlane[2],
							static_cast<size_t>((pSrcImg->width >> 1) * (pSrcImg->height >> 1)),1,fp);
					break;
				}
				case IMAGE_FORMAT_NV21:
				case IMAGE_FORMAT_NV12:
				{
					fwrite(pSrcImg->ppPlane[0],
						   static_cast<size_t>(pSrcImg->width * pSrcImg->height), 1, fp);
					fwrite(pSrcImg->ppPlane[1],
						   static_cast<size_t>(pSrcImg->width * (pSrcImg->height >> 1)), 1, fp);
					break;
				}
				case IMAGE_FORMAT_RGBA:
				{
					fwrite(pSrcImg->ppPlane[0],
						   static_cast<size_t>(pSrcImg->width * pSrcImg->height * 4), 1, fp);
					break;
				}
				default:
				{
					LOGCATE("DumpNativeImage default");
					break;
				}
			}

			fclose(fp);
			fp = NULL;
		}


	}
};


#endif //OPEN_GL_IMAGE_DEF

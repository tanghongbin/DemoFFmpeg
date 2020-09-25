//
// Created by Admin on 2020/8/25.
//

#include <jni.h>
#include "utils.h"
#include "../../../../../../../android_sdk/android_sdk/sdk/ndk-bundle/toolchains/llvm/prebuilt/windows-x86_64/sysroot/usr/include/GLES3/gl3.h"

#ifndef DEMOC_GLUTILS_H
#define DEMOC_GLUTILS_H

class GLUtilC {
public:
    static JavaVM* mVm;
    static int64_t master_audio_clock;
};

struct ThreadParams{
    _jstring *pJstring;
    _jobject *pJobject;
};

struct TestParams{
    int age;
    char * bookName;
    char * bookTitle;
};

void createThreadForPlay(JNIEnv *pEnv, _jstring *pJstring, _jobject *pJobject,jint type);

void CheckGLError(const char *pGLOperation);

void CheckGLError(const char *pGLOperation);

GLuint LoadShader(GLenum shaderType, const char *pSource);

GLuint CreateProgram(const char *pVertexShaderSource, const char *pFragShaderSource,
                     GLuint &vertexShaderHandle, GLuint &fragShaderHandle);


void DeleteProgram(GLuint program);

void CheckGLError(const char *pGLOperation);

void testLocalThread();

const char * encodeYuvToImageUtils(const char * filePath);

#endif //DEMOC_GLUTILS_H

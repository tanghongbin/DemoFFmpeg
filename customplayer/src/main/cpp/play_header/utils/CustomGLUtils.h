//
// Created by Admin on 2020/8/25.
//

#include <jni.h>
#include "utils.h"
#include "OpenGLImageDef.h"
#include "JavaVmManager.h"
#include <string>
#include <glm/gtc/matrix_transform.hpp>
#include <GLES3/gl3.h>
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <stdlib.h>
#include <mediaprocess/AbsCustomMediaPlayer.h>
#include <encoder/AbsMediaMuxer.h>


extern "C" {
#include <libavutil/frame.h>
#include <libavutil/pixfmt.h>
}

#ifndef DEMOC_GLUTILS_H
#define DEMOC_GLUTILS_H

class GLUtilC {
public:
    static JavaVM *mVm;
    static int64_t master_audio_clock;
};

typedef struct thread_parmas_struct {
    _jstring *pJstring;
    _jobject *pJobject;
} ThreadParams;

typedef struct test_params_struct {
    int age;
    char *bookName;
    char *bookTitle;
} TestParams;


void glCheckError(const char *pGLOperation);

GLuint LoadShader(GLenum shaderType, const char *pSource);

GLuint CreateProgram(const char *pVertexShaderSource, const char *pFragShaderSource);

unsigned int TextureFromFile(const std::string path, std::string &directory);

GLuint CreateProgram(const char *pVertexShaderSource, const char *pFragShaderSource,
                     GLuint &vertexShaderHandle, GLuint &fragShaderHandle);

GLuint CreateProgram(const char *pVertexShaderSource, const char *pFragShaderSource,
                     GLuint &vertexShaderHandle, GLuint &fragShaderHandle,
                     const char *geometryShaderSouce,GLuint &geometryHandle);

void DeleteProgram(GLuint program);

void testLocalThread();

long long GetSysCurrentTime();

const char *encodeYuvToImageUtils(const char *filePath);

const char *encodeYuvToImageUtils2(const char *filePath);

const char *encdoeMp4FromOfficalDemo();

const char *getCharStrFromJstring(JNIEnv *jniEnv, jstring str);

const char *getRandomStr(const char *prefix, const char *suffix);

//attribute_deprecated
const char *getRandomStr(const char *prefix, const char *suffix, const char *subDir);

jstring getJstringFromCharStr(JNIEnv *jniEnv, const char *str);

bool checkNegativeReturn(int ret, const char *string);

void NvToYUV420p(const uint8_t *image_src, uint8_t *image_dst, int image_width, int image_height);

void
setupRenderDimension(int orientation,int nativeWindowWidth, int nativeWindowHeight, int videoWidth, int videoHeight,
                     int *renderWidth, int *renderHeight);

void syslog_print(void *ptr, int level, const char *fmt, va_list vl);

void sys_log_init();

uint8_t *fillArrayToFrame(AVPixelFormat avPixelFormat, AVFrame *frame);

const int getRandomInt(int range);

static const std::string  readGLSLStrFromFile(const char *filePath);

static const std::string  readGLSLStrFromFile(const char *filename) {
    std::string absolutePath = std::string(GLSL_ROOT_DIR) + std::string(filename);
    std::ifstream ifile(absolutePath);
//将文件读入到ostringstream对象buf中
    std::ostringstream buf;
    char ch;
    while(buf&&ifile.get(ch))
        buf.put(ch);
//    LOGCATE("log buffer:%s",buffer);
    std::string value = buf.str();
//    LOGCATE("log fileStr:%s",value.c_str());
    return value;
}

static void setJniPointToJava(JNIEnv* env, const char * fieldName,const char * signature,void* p){
    jobject instance = JavaVmManager::getObjInstance();
    jfieldID jfieldId = env->GetFieldID(env->GetObjectClass(instance),fieldName,signature);
    env->SetLongField(instance, jfieldId, reinterpret_cast<jlong>(p));
}

static AbsCustomMediaPlayer* getJniPlayerFromJava(){
    bool isAttach;
    JNIEnv* env = JavaVmManager::GetEnv(&isAttach);
    jobject instance = JavaVmManager::getObjInstance();
    jfieldID jfieldId = env->GetFieldID(env->GetObjectClass(instance),"mNativePlayer","J");
    jlong point = env->GetLongField(instance,jfieldId);
//    if (isAttach){
//        JavaVmManager::detachCurrentThread();
//    }
    return reinterpret_cast<AbsCustomMediaPlayer *>(point);
}

static AbsMediaMuxer* getJniMuxerFromJava(){
    bool isAttach;
    JNIEnv* env = JavaVmManager::GetEnv(&isAttach);
    jobject instance = JavaVmManager::getObjInstance();
    jfieldID jfieldId = env->GetFieldID(env->GetObjectClass(instance),"mNativeMuxer","J");
    jlong point = env->GetLongField(instance,jfieldId);
//    if (isAttach){
//        JavaVmManager::detachCurrentThread();
//    }
    return reinterpret_cast<AbsMediaMuxer *>(point);
}


static void setBool(GLuint programId, const std::string &name, bool value) {
    glUniform1i(glGetUniformLocation(programId, name.c_str()), (int) value);
}

static void setInt(GLuint programId, const std::string &name, int value) {
    glUniform1i(glGetUniformLocation(programId, name.c_str()), value);
}

static void setFloat(GLuint programId, const std::string &name, float value) {
    glUniform1f(glGetUniformLocation(programId, name.c_str()), value);
}

static void setVec2(GLuint programId, const std::string &name, const glm::vec2 &value) {
    glUniform2fv(glGetUniformLocation(programId, name.c_str()), 1, &value[0]);
}

static void setVec2(GLuint programId, const std::string &name, float x, float y) {
    glUniform2f(glGetUniformLocation(programId, name.c_str()), x, y);
}

static void setVec3(GLuint programId, const std::string &name, const glm::vec3 &value) {
    glUniform3fv(glGetUniformLocation(programId, name.c_str()), 1, &value[0]);
}

static void setVec3(GLuint programId, const std::string &name, float x, float y, float z) {
    glUniform3f(glGetUniformLocation(programId, name.c_str()), x, y, z);
}

static void setVec4(GLuint programId, const std::string &name, const glm::vec4 &value) {
    glUniform4fv(glGetUniformLocation(programId, name.c_str()), 1, &value[0]);
}

static void setVec4(GLuint programId, const std::string &name, float x, float y, float z, float w) {
    glUniform4f(glGetUniformLocation(programId, name.c_str()), x, y, z, w);
}

static void setMat2(GLuint programId, const std::string &name, const glm::mat2 &mat) {
    glUniformMatrix2fv(glGetUniformLocation(programId, name.c_str()), 1, GL_FALSE, &mat[0][0]);
}

static void setMat3(GLuint programId, const std::string &name, const glm::mat3 &mat) {
    glUniformMatrix3fv(glGetUniformLocation(programId, name.c_str()), 1, GL_FALSE, &mat[0][0]);
}

static void setMat4(GLuint programId, const std::string &name, const glm::mat4 &mat) {
    glUniformMatrix4fv(glGetUniformLocation(programId, name.c_str()), 1, GL_FALSE, &mat[0][0]);
}

static glm::vec3 texCoordToVertexCoord(glm::vec2 texCoord) {
    return glm::vec3(2 * texCoord.x - 1, 1 - 2 * texCoord.y, 0);
}

#endif //DEMOC_GLUTILS_H

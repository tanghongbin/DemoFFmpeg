//
// Created by Admin on 2020/8/24.
//


#ifndef UTILS_LOCAL_H
#define UTILS_LOCAL_H

#include <android/log.h>
#include <string>

/**
 * 是否启用ffmpeg日志
 */
//#define ENABLE_FFMPEG_LOG true
#define ENABLE_FFMPEG_LOG false

#define ROOT_DIR "/storage/emulated/0/ffmpegtest/"
#define FILTER_IMAGE_DIR "/storage/emulated/0/ffmpegtest/filterImg/"
#define GLSL_ROOT_DIR "/storage/emulated/0/ffmpegtest/assets/glsl/"
#define MODEL3D_ROOT_DIR "/storage/emulated/0/ffmpegtest/3d_obj/"

#define COMMON_YUV_IMAGE_PATH "/storage/emulated/0/ffmpegtest/YUV_Image_840x1074.NV21"
#define LOG_TAG "AudioDemo-JNI"
#define LOGCATE(...) __android_log_print(ANDROID_LOG_DEBUG,LOG_TAG,__VA_ARGS__)
#define LOGCATE_LEVEL(level,...) ((void)__android_log_vprint(level,LOG_TAG,__VA_ARGS__))
//#define ALOG(level, TAG, ...)    ((void)__android_log_vprint(level, TAG, __VA_ARGS__))
#define MATH_PI 3.1415926535897932384626433832802

#define RENDER_DIMENSION_CALLBACK "renderDimensionCallFromJni"
#define DECODE_READY_DURATION "onDecodeReadyDuration"

// native给java通信
#define JNI_COMMUNICATE_TYPE_PREPARED 1
#define JNI_COMMUNICATE_TYPE_VIDEO_SIZE_CHANGED 2
#define JNI_COMMUNICATE_TYPE_SEEK_PROGRESS_CHANGED 3
#define JNI_COMMUNICATE_TYPE_COMPLETE 4
#define JNI_COMMUNICATE_TYPE_ERROR 5
#define JNI_COMMUNICATE_TYPE_DURATION 6


typedef void (*PrepareCall) (long);

static std::string getModel3DPath(const char * fileName){
    std::string result = std::string (MODEL3D_ROOT_DIR) + std::string (fileName);
    return result;
}

#endif
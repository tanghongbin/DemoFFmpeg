//
// Created by Admin on 2020/8/24.
//


#ifndef UTILS_LOCAL_H
#define UTILS_LOCAL_H

#include <android/log.h>

#define ENABLE_FFMPEG_LOG false
#define LOG_TAG "AudioDemo-JNI"
#define LOGCATE(...) __android_log_print(ANDROID_LOG_DEBUG,LOG_TAG,__VA_ARGS__)
#define LOGCATE_LEVEL(level,...) ((void)__android_log_vprint(level,LOG_TAG,__VA_ARGS__))
//#define ALOG(level, TAG, ...)    ((void)__android_log_vprint(level, TAG, __VA_ARGS__))
#define MATH_PI 3.1415926535897932384626433832802

#endif

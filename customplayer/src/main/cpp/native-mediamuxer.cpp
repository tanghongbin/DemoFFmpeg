//
// Created by Admin on 2020/9/27.
//


// 视频录制功能,ffmpeg,hw

#include <jni.h>
#include <mediaprocess/MediaCodecPlayer.h>
#include <utils/JavaVmManager.h>
#include <utils/MsgLoopHelper.h>
#include <encoder/FFmpegMediaMuxer.h>
#include <utils/AudioRecordPlayHelper.h>
#include <encoder/HwMediaMuxer.h>

#include "mediaprocess/AbsCustomMediaPlayer.h"
#include "mediaprocess/FFmpegMediaPlayer.h"
#include "play_header/utils/CustomSafeBlockQueue.h"
#include <MathFunctions.h>
#include <encoder/RtmpLiveMuxer.h>
#include <encoder/ShortVideoMuxer.h>
#include <encoder/OutputDisplayHelper.h>
#include <utils/NativeMediaConstant.h>

//com.testthb.customplayer.player
#define _NATIVE_MUXER_CLASS "com/testthb/customplayer/player/CustomMediaRecorder"


#ifdef __cplusplus


extern "C" {
#endif

#include <libavcodec/jni.h>

OutputDisplayHelper* getJniOutputHelperFromJava(){
    jlong result = getJniPointFromJava("mNativeOutputHelper");
    if (result == 0L) return nullptr;
    return reinterpret_cast<OutputDisplayHelper *>(result);
}

/***
 *  ============================== 播放器 GL callback ===============================
 */

JNIEXPORT void JNICALL native_OnSurfaceCreated(JNIEnv *env, jobject instance) {
    auto *outputDisplayHelper = getJniOutputHelperFromJava();
    if (outputDisplayHelper) {
        outputDisplayHelper->OnSurfaceCreate();
    }
}

JNIEXPORT void JNICALL native_OnSurfaceChanged(JNIEnv *env, jobject instance,jint oretenation,jint width,jint height) {
    auto *outputDisplayHelper = getJniOutputHelperFromJava();
    if (outputDisplayHelper) {
        outputDisplayHelper->OnSurfaceChanged(width,height);
    }
}

JNIEXPORT void JNICALL native_OnDrawFrame(JNIEnv *env, jobject instance) {
    auto *outputDisplayHelper = getJniOutputHelperFromJava();
    if (outputDisplayHelper) {
        outputDisplayHelper->OnDrawFrame();
    }
}

JNIEXPORT void JNICALL native_OnDestroy(JNIEnv *env, jobject instance) {
    AbsMediaMuxer *mediaMuxer = getJniMuxerFromJava();
    if (mediaMuxer){
        setJniPointToJava(env,"mNativeMuxer","J", nullptr);
        mediaMuxer->Destroy();
        delete mediaMuxer;
    }
    auto *outputDisplayHelper = getJniOutputHelperFromJava();
    if (outputDisplayHelper) {
        setJniPointToJava(env,"mNativeOutputHelper","J", nullptr);
        delete outputDisplayHelper;
    }
    JavaVmManager::destroyInstance(env);
    MsgLoopHelper::destroyInstance();
    LOGCATE("native_OnDestroy destroy all");
}

/****
 * ============================   编码视频，音频，合并部分  ==========================
 * *****/

/***
 *
 * @param env
 * @param instance
 * @param type  11-ffmpeg 编码，12- 硬编码 , 13 - 直播推流 , 14- 短视频录制
 */
JNIEXPORT void JNICALL native_init_muxer(JNIEnv *env, jobject instance,jint type) {
    JavaVmManager::initVm(env);
    JavaVmManager::setInstance(env,instance);
    if (av_jni_set_java_vm(JavaVmManager::getJavaVM(), NULL) < 0) {
        LOGCATE("get javavm error");
    }
    if (ENABLE_FFMPEG_LOG) {
        LOGCATE("system log has been init");
        sys_log_init();
    }
    MsgLoopHelper::initMsgLoop();
    AbsMediaMuxer *mediaMuxer = nullptr;
    if (type == MUXER_RECORD_FFMPEG) {
        mediaMuxer = FFmpegMediaMuxer::getInstace();
    } else if (type == MUXER_RECORD_HW) {
        mediaMuxer = HwMediaMuxer::getInstace();
    } else if (type == MUXER_RTMP){
        mediaMuxer = RtmpLiveMuxer::getInstance();
    } else if (type == MUXER_SHORT_VIDEO) {
        mediaMuxer = ShortVideoMuxer::getInstance();
    } else {
        mediaMuxer = FFmpegMediaMuxer::getInstace();
    }
    OutputDisplayHelper::getInstance()->init();
    setJniPointToJava( env,"mNativeOutputHelper","J" ,OutputDisplayHelper::getInstance());
    setJniPointToJava(env,"mNativeMuxer","J" ,mediaMuxer);
}

JNIEXPORT void JNICALL native_initEncode(JNIEnv *env, jobject instance, jstring path) {
    AbsMediaMuxer *mediaMuxer = getJniMuxerFromJava();
    if (mediaMuxer == nullptr) return;
    const char* resultPath = getCharStrFromJstring(env,path);
    ReceiveAvOriginalData receiveAvOriginalData;
    receiveAvOriginalData.audioCall = nullptr;
    receiveAvOriginalData.videoCall = nullptr;
    mediaMuxer->getInAvDataFunc(&receiveAvOriginalData);
    OutputDisplayHelper::getInstance()->setOutputAudioListener(receiveAvOriginalData.audioCall);
    OutputDisplayHelper::getInstance()->setOutputVideoListener(receiveAvOriginalData.videoCall);
    mediaMuxer->init(resultPath);
    LOGCATE(" init native_initEncode");
}


JNIEXPORT void JNICALL native_startEncode(JNIEnv *env, jobject instance) {
    AbsMediaMuxer *mediaMuxer = getJniMuxerFromJava();
    if (mediaMuxer == nullptr) return;
    mediaMuxer->Start();
}

JNIEXPORT void JNICALL native_stopEncode(JNIEnv *env, jobject instance) {
    AbsMediaMuxer *mediaMuxer = getJniMuxerFromJava();
    if (mediaMuxer == nullptr) return;
    mediaMuxer->Stop();
}

JNIEXPORT void JNICALL native_onCameraFrameDataValible(JNIEnv *env, jobject instance,jint type,jbyteArray imageData) {
    LOGCATE("still recive frame data");
    auto *outputDisplayHelper = getJniOutputHelperFromJava();
    if (!outputDisplayHelper) return;
    // 如果是扩展纹理，不做处理
    if (type == 5) return;

    jbyte *data = env->GetByteArrayElements(imageData, 0);
    if (!data) return;
    NativeOpenGLImage openGlImage;
    if (type == 2) {
        openGlImage.width = 1280;
        openGlImage.height = 720;
        openGlImage.format = IMAGE_FORMAT_I420;
        openGlImage.ppPlane[0] = reinterpret_cast<uint8_t *>(data);
        openGlImage.ppPlane[1] = reinterpret_cast<uint8_t *>(data) + openGlImage.width * openGlImage.height;
        openGlImage.ppPlane[2] = reinterpret_cast<uint8_t *>(data) + openGlImage.width * openGlImage.height * 5 / 4;
        openGlImage.pLineSize[0] = openGlImage.width;
        openGlImage.pLineSize[1] = openGlImage.width/2;
        openGlImage.pLineSize[2] = openGlImage.width/2;
    }
    outputDisplayHelper->OnCameraFrameDataValible(type,&openGlImage);
    env->ReleaseByteArrayElements(imageData, data, 0);
}


JNIEXPORT void JNICALL native_audioData(JNIEnv *env, jobject instance,jbyteArray audioData,jint length) {
    auto *outputDisplayHelper = getJniOutputHelperFromJava();
    if (!outputDisplayHelper) return;

    jbyte *data = env->GetByteArrayElements(audioData, 0);
    if (!data) return;
    outputDisplayHelper ->OnAudioData(reinterpret_cast<uint8_t *>(data), length);
    env->ReleaseByteArrayElements(audioData, data, 0);
}

JNIEXPORT void JNICALL native_updateMatrix(JNIEnv *env, jobject instance,jfloatArray javadata) {
    auto *outputDisplayHelper = getJniOutputHelperFromJava();
    if (!outputDisplayHelper) return;

    jfloat *data = env->GetFloatArrayElements(javadata, 0);
    if (!data) return;
    outputDisplayHelper ->UpdateOESMartix(reinterpret_cast<float *>(data));
    env->ReleaseFloatArrayElements(javadata, data, 0);
}

JNIEXPORT void JNICALL native_configAudioParams(JNIEnv *env, jobject instance,jint sampleHz,jint channels) {
    AbsMediaMuxer *mediaMuxer = getJniMuxerFromJava();
    if (!mediaMuxer)
        return;
    mediaMuxer->configAudioPrams(sampleHz,channels);
}


JNIEXPORT void JNICALL native_setSpeed(JNIEnv *env, jobject instance,jdouble speed) {
    AbsMediaMuxer *mediaMuxer = getJniMuxerFromJava();
    if (mediaMuxer) {
        mediaMuxer->SetPeed(speed);
    }

    auto *outputDisplayHelper = getJniOutputHelperFromJava();
    if (outputDisplayHelper) {
        outputDisplayHelper->setSpeed(speed);
    }
}




/***
 *  ===================================   注释方法部分   ==================================
 */

static JNINativeMethod g_RenderMethods[] = {
        {"native_OnSurfaceCreated",               "()V",            (void *) (native_OnSurfaceCreated)},
        {"native_OnSurfaceChanged",               "(III)V",            (void *) (native_OnSurfaceChanged)},
        {"native_OnDrawFrame",               "()V",            (void *) (native_OnDrawFrame)},
        {"native_OnDestroy",               "()V",            (void *) (native_OnDestroy)},
        {"native_init_muxer",               "(I)V",            (void *) (native_init_muxer)},

        {"native_initEncode",               "(Ljava/lang/String;)V",            (void *) (native_initEncode)},
        {"native_stopEncode",               "()V",            (void *) (native_stopEncode)},
        {"native_startEncode",               "()V",            (void *) (native_startEncode)},
        {"native_onCameraFrameDataValible",               "(I[B)V",            (void *) (native_onCameraFrameDataValible)},
        {"native_audioData",               "([BI)V",            (void *) (native_audioData)},
        {"native_updateMatrix",               "([F)V",            (void *) (native_updateMatrix)},
        {"native_configAudioParams",               "(II)V",            (void *) (native_configAudioParams)},
        {"native_setSpeed",               "(D)V",            (void *) (native_setSpeed)},


};




static int RegisterNativeMethods(JNIEnv *env, const char *className, JNINativeMethod *methods, int methodNum) {
    jclass clazz = env->FindClass(className);
    if (clazz == NULL) {
        return JNI_FALSE;
    }
    if (env->RegisterNatives(clazz, methods, methodNum) < 0) {
        return JNI_FALSE;
    }
    return JNI_TRUE;
}

static void UnregisterNativeMethods(JNIEnv *env, const char *className) {
    jclass clazz = env->FindClass(className);
    if (clazz == NULL) {
        return;
    }
    if (env != NULL) {
        env->UnregisterNatives(clazz);
    }
}

// call this func when loading lib
extern "C" jint JNI_OnLoad(JavaVM *jvm, void *p) {
//    LOGCATE("===== JNI_OnLoad =====");
    jint jniRet = JNI_ERR;
    JNIEnv *env = NULL;
    if (jvm->GetEnv((void **) (&env), JNI_VERSION_1_6) != JNI_OK) {
        return jniRet;
    }


    jint regRet = RegisterNativeMethods(env, _NATIVE_MUXER_CLASS, g_RenderMethods,
                                        sizeof(g_RenderMethods) /
                                        sizeof(g_RenderMethods[0]));
    if (regRet != JNI_TRUE) {
        return JNI_ERR;
    }
    return JNI_VERSION_1_6;
}


extern "C" void JNI_OnUnload(JavaVM *jvm, void *p) {
    JNIEnv *env = NULL;
    if (jvm->GetEnv((void **) (&env), JNI_VERSION_1_6) != JNI_OK) {
        return;
    }
    UnregisterNativeMethods(env, _NATIVE_MUXER_CLASS);
}

#ifdef __cplusplus
}
#endif
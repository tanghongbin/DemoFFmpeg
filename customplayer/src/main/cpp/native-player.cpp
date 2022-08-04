//
// Created by Admin on 2020/9/27.
//


// 测试ffmpeg的编解码，格式转换，混合和解混合，推拉流

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
#define _NATIVE_PLAYER_CLASS "com/testthb/customplayer/player/CustomMediaPlayer"


#ifdef __cplusplus


extern "C" {
#endif

#include <libavcodec/jni.h>

/***
 *  ============================== 播放器 GL callback ===============================
 */

JNIEXPORT void JNICALL native_OnSurfaceCreated(JNIEnv *env, jobject instance) {
    AbsCustomMediaPlayer *mediaPlayer = getJniPlayerFromJava();
    if (mediaPlayer != NULL) mediaPlayer->OnSurfaceCreated();
}

JNIEXPORT void JNICALL native_OnSurfaceChanged(JNIEnv *env, jobject instance,jint oretenation,jint width,jint height) {
    AbsCustomMediaPlayer *mediaPlayer = getJniPlayerFromJava();
    if (mediaPlayer) mediaPlayer->OnSurfaceChanged(oretenation,width,height);
}

JNIEXPORT void JNICALL native_OnDrawFrame(JNIEnv *env, jobject instance) {
    AbsCustomMediaPlayer *mediaPlayer = getJniPlayerFromJava();
    if (mediaPlayer != NULL) mediaPlayer->OnDrawFrame();
}

JNIEXPORT void JNICALL native_OnDestroy(JNIEnv *env, jobject instance) {
    AbsCustomMediaPlayer *mediaPlayer = getJniPlayerFromJava();
    if (mediaPlayer) {
        setJniPointToJava(env,"mNativePlayer","J", nullptr);
        mediaPlayer->Destroy();
        delete mediaPlayer;
    }
    JavaVmManager::destroyInstance(env);
    MsgLoopHelper::destroyInstance();
}

JNIEXPORT void JNICALL native_init_player(JNIEnv *env, jobject instance,jint playerType) {
    JavaVmManager::initVm(env);
    JavaVmManager::setInstance(env,instance);
    if (av_jni_set_java_vm(JavaVmManager::getJavaVM(), NULL) < 0) {
        LOGCATE("av_jni_set_java_vm error");
    }
    if (ENABLE_FFMPEG_LOG) {
        LOGCATE("system log has been init");
        sys_log_init();
    }
    MsgLoopHelper::initMsgLoop();
    AbsCustomMediaPlayer *mediaPlayer;
    if (playerType == PLAYER_FFMPEG) {
        mediaPlayer = new FFmpegMediaPlayer;
    } else {
        mediaPlayer = MediaCodecPlayer::getInstance();
    }
    setJniPointToJava(env,"mNativePlayer","J", mediaPlayer);
    mediaPlayer->Init();
}


JNIEXPORT void JNICALL native_prepare(JNIEnv *env, jobject instance) {
    AbsCustomMediaPlayer *mediaPlayer = getJniPlayerFromJava();
    LOGCATE("native_prepare:%p",mediaPlayer);
    if (mediaPlayer == NULL) return;
    mediaPlayer->Prepare();
}

JNIEXPORT void JNICALL native_start(JNIEnv *env, jobject instance) {
    AbsCustomMediaPlayer *mediaPlayer = getJniPlayerFromJava();
    if (mediaPlayer == NULL) return;
    mediaPlayer->Start();
}

JNIEXPORT void JNICALL native_stop(JNIEnv *env, jobject instance) {
    AbsCustomMediaPlayer *mediaPlayer = getJniPlayerFromJava();
    if (mediaPlayer == NULL) return;
    mediaPlayer->Stop();
}

JNIEXPORT void JNICALL native_resetPlay(JNIEnv *env, jobject instance) {
    AbsCustomMediaPlayer *mediaPlayer = getJniPlayerFromJava();
    if (mediaPlayer == NULL) return;
    mediaPlayer->Reset();
}



JNIEXPORT void JNICALL native_seekTo(JNIEnv *env, jobject instance,jint seekProgress) {
    AbsCustomMediaPlayer *mediaPlayer = getJniPlayerFromJava();
    if (mediaPlayer == NULL) return;
    mediaPlayer->SeekTo(seekProgress);
}

JNIEXPORT void JNICALL native_setDataUrl(JNIEnv *env, jobject instance,jstring url) {
    AbsCustomMediaPlayer *mediaPlayer = getJniPlayerFromJava();
    if (mediaPlayer != NULL)
        mediaPlayer->SetDataUrl(getCharStrFromJstring(env,url));
}


JNIEXPORT void JNICALL native_replay(JNIEnv *env, jobject instance) {
    AbsCustomMediaPlayer *mediaPlayer = getJniPlayerFromJava();
    if (mediaPlayer != NULL)
        mediaPlayer->Replay();
}


JNIEXPORT void JNICALL native_applyEfforts(JNIEnv *env, jobject instance,jstring url) {
    AbsCustomMediaPlayer *mediaPlayer = getJniPlayerFromJava();
    if (mediaPlayer != NULL)
        mediaPlayer->ApplyEfforts(getCharStrFromJstring(env,url));
}


/***
 *  ===================================   注释方法部分   ==================================
 */

static JNINativeMethod g_RenderMethods[] = {
        {"native_OnSurfaceCreated",               "()V",            (void *) (native_OnSurfaceCreated)},
        {"native_OnSurfaceChanged",               "(III)V",            (void *) (native_OnSurfaceChanged)},
        {"native_OnDrawFrame",               "()V",            (void *) (native_OnDrawFrame)},
        {"native_OnDestroy",               "()V",            (void *) (native_OnDestroy)},

        {"native_init_player",               "(I)V",            (void *) (native_init_player)},
        {"native_prepare",               "()V",            (void *) (native_prepare)},
        {"native_start",               "()V",            (void *) (native_start)},
        {"native_stop",               "()V",            (void *) (native_stop)},
        {"native_resetPlay",               "()V",            (void *) (native_resetPlay)},
        {"native_seekTo",               "(I)V",            (void *) (native_seekTo)},
        {"native_setDataUrl",               "(Ljava/lang/String;)V",            (void *) (native_setDataUrl)},
        {"native_replay",               "()V",            (void *) (native_replay)},
        {"native_applyEfforts",               "(Ljava/lang/String;)V",            (void *) (native_applyEfforts)},



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


    jint regRet = RegisterNativeMethods(env, _NATIVE_PLAYER_CLASS, g_RenderMethods,
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
    UnregisterNativeMethods(env, _NATIVE_PLAYER_CLASS);
}

#ifdef __cplusplus
}
#endif
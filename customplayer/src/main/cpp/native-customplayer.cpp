//
// Created by Admin on 2020/9/27.
//


// 测试ffmpeg的编解码，格式转换，混合和解混合，推拉流

#include <jni.h>
#include <mediaprocess/MediaCodecPlayer.h>
#include <utils/JavaVmManager.h>
#include <utils/MsgLoopHelper.h>

#include "mediaprocess/AbsCustomMediaPlayer.h"
#include "mediaprocess/FFmpegMediaPlayer.h"
#include "play_header/utils/CustomSafeQueue.h"


#define NATIVE_RENDER_CLASS_ "com/example/customplayer/player/CustomPlayer"


#ifdef __cplusplus


extern "C" {
#endif

#include <libavcodec/jni.h>

JNIEXPORT jstring JNICALL nativeGetInfo(JNIEnv *env, jobject instance) {
    MsgLoopHelper::sendMsg(Message::obtain(5,2,3));
    return env->NewStringUTF("你好吗，朋友");
}


/***
 *  ==============================  GL callback ===============================
 */

JNIEXPORT void JNICALL native_OnSurfaceCreated(JNIEnv *env, jobject instance) {

}

JNIEXPORT void JNICALL native_OnSurfaceChanged(JNIEnv *env, jobject instance,jint width,jint height) {
    AbsCustomMediaPlayer *mediaPlayer = getJniPlayerFromJava();
    if (mediaPlayer != NULL) mediaPlayer->OnSurfaceChanged(width,height);
}

JNIEXPORT void JNICALL native_OnDrawFrame(JNIEnv *env, jobject instance) {
    AbsCustomMediaPlayer *mediaPlayer = getJniPlayerFromJava();
    if (mediaPlayer != NULL)
    mediaPlayer->OnDrawFrame();
}

JNIEXPORT void JNICALL native_OnDestroy(JNIEnv *env, jobject instance) {
    AbsCustomMediaPlayer *mediaPlayer = getJniPlayerFromJava();
    if (mediaPlayer == NULL) return;
    mediaPlayer->Destroy();
    delete mediaPlayer;
    mediaPlayer = NULL;
    JavaVmManager::destroyInstance();
    MsgLoopHelper::destroyInstance();
}

JNIEXPORT void JNICALL native_init(JNIEnv *env, jobject instance) {
    JavaVmManager::setInstance(env,instance);
    FFmpegMediaPlayer *mediaPlayer = new FFmpegMediaPlayer;
    setJniPlayerToJava(env,mediaPlayer);
    mediaPlayer->Init();
    MsgLoopHelper::initMsgLoop();
    LOGCATE("has enter env:%p instance:%p",env,instance);
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

JNIEXPORT void JNICALL native_seekTo(JNIEnv *env, jobject instance,jlong seekProgress) {

}

JNIEXPORT void JNICALL native_setDataUrl(JNIEnv *env, jobject instance,jstring url) {
    AbsCustomMediaPlayer *mediaPlayer = getJniPlayerFromJava();
    if (mediaPlayer != NULL)
        mediaPlayer->SetDataUrl(getCharStrFromJstring(env,url));
}


/***
 *  ===================================   注释方法部分   ==================================
 */

static JNINativeMethod g_RenderMethods[] = {
        {"nativeGetInfo",               "()Ljava/lang/String;",            (void *) (nativeGetInfo)},
        {"native_OnSurfaceCreated",               "()V",            (void *) (native_OnSurfaceCreated)},
        {"native_OnSurfaceChanged",               "(II)V",            (void *) (native_OnSurfaceChanged)},
        {"native_OnDrawFrame",               "()V",            (void *) (native_OnDrawFrame)},
        {"native_OnDestroy",               "()V",            (void *) (native_OnDestroy)},

        {"native_init",               "()V",            (void *) (native_init)},
        {"native_prepare",               "()V",            (void *) (native_prepare)},
        {"native_start",               "()V",            (void *) (native_start)},
        {"native_stop",               "()V",            (void *) (native_stop)},
        {"native_seekTo",               "(J)V",            (void *) (native_seekTo)},
        {"native_setDataUrl",               "(Ljava/lang/String;)V",            (void *) (native_setDataUrl)}
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


    jint regRet = RegisterNativeMethods(env, NATIVE_RENDER_CLASS_, g_RenderMethods,
                                        sizeof(g_RenderMethods) /
                                        sizeof(g_RenderMethods[0]));
    if (regRet != JNI_TRUE) {
        return JNI_ERR;
    }
    JavaVmManager::initVm(env);
    if (av_jni_set_java_vm(jvm, NULL) < 0) {
        return JNI_ERR;
    }
    if (ENABLE_FFMPEG_LOG) {
        LOGCATE("system log has been init");
        sys_log_init();
    }
    return JNI_VERSION_1_6;
}


extern "C" void JNI_OnUnload(JavaVM *jvm, void *p) {
    JNIEnv *env = NULL;
    if (jvm->GetEnv((void **) (&env), JNI_VERSION_1_6) != JNI_OK) {
        return;
    }

    UnregisterNativeMethods(env, NATIVE_RENDER_CLASS_);

}

#ifdef __cplusplus
}
#endif
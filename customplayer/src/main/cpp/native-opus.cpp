       //
// Created by Admin on 2020/9/27.
//


// opus 的录制与播放

#include <jni.h>
#include <utils/JavaVmManager.h>
#include <utils/MsgLoopHelper.h>
#include "play_header/utils/CustomSafeBlockQueue.h"
#include <MathFunctions.h>
#include <utils/NativeMediaConstant.h>
#include <utils/CustomGLUtils.h>
#include <opus_util/NativeOpusHelper.h>

//com.testthb.customplayer.player
#define _NATIVE_PLAYER_CLASS "com/testthb/customplayer/opus/OpusHelper"


#ifdef __cplusplus


extern "C" {
#endif


NativeOpusHelper* getJniOpusPointFromJava(){
    jlong result = getJniPointFromJava("mNativeOpusHelper");
    if (result == 0L) return nullptr;
    return reinterpret_cast<NativeOpusHelper *>(result);
}
/***
 *  ============================== opus 操作部分  ===============================
 */
JNIEXPORT void JNICALL native_initOpus(JNIEnv *env, jobject instance,jint playerType) {
    JavaVmManager::initVm(env);
    JavaVmManager::setInstance(env,instance);
    MsgLoopHelper::initMsgLoop();
    auto* opusHelper = new NativeOpusHelper;
    opusHelper->init();
    setJniPointToJava(env,"mNativeOpusHelper","J",opusHelper);
}


JNIEXPORT void JNICALL native_startEncode(JNIEnv *env, jobject instance) {
    NativeOpusHelper* opusHelper = getJniOpusPointFromJava();
    if (opusHelper )opusHelper->startEncode();
}


JNIEXPORT void JNICALL native_stopEncode(JNIEnv *env, jobject instance) {
    NativeOpusHelper* opusHelper = getJniOpusPointFromJava();
    if (opusHelper )opusHelper->stopEncode();
}


JNIEXPORT void JNICALL native_startPlayOpus(JNIEnv *env, jobject instance) {
    NativeOpusHelper* opusHelper = getJniOpusPointFromJava();
    if (opusHelper )opusHelper->startPlay();
}


JNIEXPORT void JNICALL native_destroyOpus(JNIEnv *env, jobject instance) {
    NativeOpusHelper* opusHelper = getJniOpusPointFromJava();
    if (opusHelper ) opusHelper->destroy();
    delete opusHelper;
    setJniPointToJava(env,"mNativeOpusHelper","J", nullptr);
}



/***
 *  ===================================   注释方法部分   ==================================
 */

static JNINativeMethod g_RenderMethods[] = {
        {"native_initOpus",               "(I)V",            (void *) (native_initOpus)},
        {"native_startEncode",               "()V",            (void *) (native_startEncode)},
        {"native_stopEncode",               "()V",            (void *) (native_stopEncode)},
        {"native_startPlayOpus",               "()V",            (void *) (native_startPlayOpus)},
        {"native_destroyOpus",               "()V",            (void *) (native_destroyOpus)},
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
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

#define NATIVE_RENDER_CLASS_ "com/example/customplayer/player/CustomMediaController"


#ifdef __cplusplus


extern "C" {
#endif

#include <libavcodec/jni.h>

JNIEXPORT jstring JNICALL nativeGetInfo(JNIEnv *env, jobject instance) {
    MsgLoopHelper::sendMsg(Message::obtain(5,2,3));
    return env->NewStringUTF("你好吗，朋友");
}


/***
 *  ============================== 播放器 GL callback ===============================
 */

JNIEXPORT void JNICALL native_OnSurfaceCreated(JNIEnv *env, jobject instance) {
    AbsCustomMediaPlayer *mediaPlayer = getJniPlayerFromJava();
    if (mediaPlayer != NULL) mediaPlayer->OnSurfaceCreated();

    AbsMediaMuxer *mediaMuxer = getJniMuxerFromJava();
    if (mediaMuxer) mediaMuxer->OnSurfaceCreate();
}

JNIEXPORT void JNICALL native_OnSurfaceChanged(JNIEnv *env, jobject instance,jint oretenation,jint width,jint height) {
    AbsCustomMediaPlayer *mediaPlayer = getJniPlayerFromJava();
    if (mediaPlayer) mediaPlayer->OnSurfaceChanged(oretenation,width,height);

    AbsMediaMuxer *mediaMuxer = getJniMuxerFromJava();
    if (mediaMuxer) mediaMuxer->OnSurfaceChanged(width,height);
}

JNIEXPORT void JNICALL native_OnDrawFrame(JNIEnv *env, jobject instance) {
    AbsCustomMediaPlayer *mediaPlayer = getJniPlayerFromJava();
    if (mediaPlayer != NULL) mediaPlayer->OnDrawFrame();

    AbsMediaMuxer *mediaMuxer = getJniMuxerFromJava();
    if (mediaMuxer != NULL) mediaMuxer->OnDrawFrame();
}

JNIEXPORT void JNICALL native_OnDestroy(JNIEnv *env, jobject instance) {
    AbsCustomMediaPlayer *mediaPlayer = getJniPlayerFromJava();
    if (mediaPlayer) {
        setJniPointToJava(env,"mNativePlayer","J", nullptr);
        mediaPlayer->Destroy();
        delete mediaPlayer;
    }
    AbsMediaMuxer *mediaMuxer = getJniMuxerFromJava();
    LOGCATE("log mediamuxer :%p",mediaMuxer);
    if (mediaMuxer){
        setJniPointToJava(env,"mNativeMuxer","J", nullptr);
        mediaMuxer->Destroy();
        delete mediaMuxer;
    }
    JavaVmManager::destroyInstance(env);
    MsgLoopHelper::destroyInstance();
    LOGCATE("destroy all over");
}

JNIEXPORT void JNICALL native_init_player(JNIEnv *env, jobject instance) {
    JavaVmManager::setInstance(env,instance);
    MsgLoopHelper::initMsgLoop();
    FFmpegMediaPlayer *mediaPlayer = new FFmpegMediaPlayer;
    setJniPointToJava(env,"mNativePlayer","J", mediaPlayer);
    mediaPlayer->Init();
    LOGCATE("has enter env:%p instance:%p  测试数字:%lf   文字:%s  nubmer:%d"
            ,env,instance,mysqrt(592.32),getInfo(),computeFour(100));
}


/***
 *
 * @param env
 * @param instance
 * @param type  1-ffmpeg 编码，2- 硬编码
 */
JNIEXPORT void JNICALL native_init_muxer(JNIEnv *env, jobject instance,jint type) {
    JavaVmManager::setInstance(env,instance);
    MsgLoopHelper::initMsgLoop();
    AbsMediaMuxer *mediaMuxer;
    if (type == 1) {
        mediaMuxer = FFmpegMediaMuxer::getInstace();
    } else {
        mediaMuxer = HwMediaMuxer::getInstace();
    }
    setJniPointToJava(env,"mNativeMuxer","J" ,mediaMuxer);
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

/****
 * ============================   编码视频，音频，合并部分  ==========================
 * *****/

JNIEXPORT void JNICALL native_startEncode(JNIEnv *env, jobject instance) {
    AbsMediaMuxer *mediaMuxer = getJniMuxerFromJava();
    if (mediaMuxer == NULL) return;

    char resultPath[128];
    const char * folder = "/storage/emulated/0/ffmpegtest/encodeVideos";
    sprintf(resultPath,"%s/%lld%s",folder,GetSysCurrentTime(),"-randow.mp4");
    createFolderIfNotExist(folder);
    mediaMuxer->init(resultPath);
}

JNIEXPORT void JNICALL native_onCameraFrameDataValible(JNIEnv *env, jobject instance,jint type,jbyteArray imageData) {
    AbsMediaMuxer *mediaMuxer = getJniMuxerFromJava();
    if (!mediaMuxer)
        return;

//    LOGCATE("log native_onCameraFrameDataValible:%p",mediaMuxer);

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
    mediaMuxer->OnCameraFrameDataValible(type,&openGlImage);

    env->ReleaseByteArrayElements(imageData, data, 0);
}


JNIEXPORT void JNICALL native_audioData(JNIEnv *env, jobject instance,jbyteArray audioData,jint length) {
    AbsMediaMuxer *mediaMuxer = getJniMuxerFromJava();
    if (!mediaMuxer)
        return;
    jbyte *data = env->GetByteArrayElements(audioData, 0);
    if (!data) return;
    mediaMuxer ->OnAudioData(reinterpret_cast<uint8_t *>(data), length);
    env->ReleaseByteArrayElements(audioData, data, 0);
}




/***
 *  ===================================   注释方法部分   ==================================
 */

static JNINativeMethod g_RenderMethods[] = {
        {"nativeGetInfo",               "()Ljava/lang/String;",            (void *) (nativeGetInfo)},
        {"native_OnSurfaceCreated",               "()V",            (void *) (native_OnSurfaceCreated)},
        {"native_OnSurfaceChanged",               "(III)V",            (void *) (native_OnSurfaceChanged)},
        {"native_OnDrawFrame",               "()V",            (void *) (native_OnDrawFrame)},
        {"native_OnDestroy",               "()V",            (void *) (native_OnDestroy)},

        {"native_init_player",               "()V",            (void *) (native_init_player)},
        {"native_init_muxer",               "(I)V",            (void *) (native_init_muxer)},
        {"native_prepare",               "()V",            (void *) (native_prepare)},
        {"native_start",               "()V",            (void *) (native_start)},
        {"native_stop",               "()V",            (void *) (native_stop)},
        {"native_seekTo",               "(I)V",            (void *) (native_seekTo)},
        {"native_setDataUrl",               "(Ljava/lang/String;)V",            (void *) (native_setDataUrl)},

        {"native_startEncode",               "()V",            (void *) (native_startEncode)},
        {"native_onCameraFrameDataValible",               "(I[B)V",            (void *) (native_onCameraFrameDataValible)},
        {"native_audioData",               "([BI)V",            (void *) (native_audioData)},
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
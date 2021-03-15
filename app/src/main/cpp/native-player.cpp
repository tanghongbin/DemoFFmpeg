//
// Created by Admin on 2020/9/27.
//

#include <jni.h>
#include <CustomGLUtils.h>
#include <YuvToImageRender.h>
#include <OpenGLFFmpegRender.h>
#include <PlayMp4Instance.h>
#include <VideoGLRender.h>
#include <PlayMp4Practice.h>

#define NATIVE_RENDER_CLASS_ "com/example/democ/render/FFmpegOpenGLRender"


#ifdef __cplusplus
extern "C" {
#endif
/*
 * Class:     com_byteflow_app_MyNativeRender
 * Method:    native_OnSurfaceCreated
 * Signature: ()V
 */
JNIEXPORT void JNICALL native_OnSurfaceCreated(JNIEnv *env, jobject instance) {
    VideoGLRender::GetInstance() -> OnSurfaceCreated();
}

/*
 * Class:     com_byteflow_app_MyNativeRender
 * Method:    native_OnSurfaceChanged
 * Signature: (II)V
 */
JNIEXPORT void JNICALL native_OnSurfaceChanged
        (JNIEnv *env, jobject instance, jint width, jint height) {
    VideoGLRender::GetInstance() -> OnSurfaceChanged(width,height);
//    OpenGLFFmpegRender::getInstance() -> onSurfaceChanged(width,height);
//    YuvToImageRender::mWindowWidth = width;
//    YuvToImageRender::mWindowHeight = height;
}

/*
 * Class:     com_byteflow_app_MyNativeRender
 * Method:    native_OnDrawFrame
 * Signature: ()V
 */
JNIEXPORT void JNICALL native_OnDrawFrame(JNIEnv *env, jobject instance) {
    VideoGLRender::GetInstance() -> OnDrawFrame();
}

PlayMp4Instance* playMp4Instance;

PlayMp4Practice* playMp4Practice;

/**
 * 播放本地MP4文件
 * @param env
 * @param instance
 */
//JNIEXPORT void JNICALL playMP4(JNIEnv *env, jobject instance,jstring url,jobject surface,jint type) {
//    LOGCATE("prepare play mp4");
//    LOGCATE("prepare play mp4");
//    if (playMp4Instance != nullptr){
//        playMp4Instance->unInit();
//        delete playMp4Instance;
//        playMp4Instance = nullptr;
//    }
//    playMp4Instance = new PlayMp4Instance();
//    const char * playUrl = env->GetStringUTFChars(url,0);
//    LOGCATE("prepare init mp4 , detected address %s",playUrl);
//    playMp4Instance->init(playUrl,env,instance,surface,type);
//}


JNIEXPORT void JNICALL playMP4(JNIEnv *env, jobject instance,jstring url,jobject surface,jint type) {
    LOGCATE("prepare play mp4");
    playMp4Practice = new PlayMp4Practice();
    const char * playUrl = env->GetStringUTFChars(url,0);
    LOGCATE("prepare init mp4 , detected address %s",playUrl);
    playMp4Practice->init(playUrl,env,instance,surface,1);
    playMp4Practice->init(playUrl, env, instance, surface, type);
}

JNIEXPORT void JNICALL native_SetImageData
        (JNIEnv *env, jobject instance, jint format, jint width, jint height,
         jbyteArray imageData) {
    int len = env->GetArrayLength(imageData);
    uint8_t *buf = new uint8_t[len];
    env->GetByteArrayRegion(imageData, 0, len, reinterpret_cast<jbyte *>(buf));
    OpenGLFFmpegRender::getInstance()->SetImageData(format, width, height, buf);
    delete[] buf;
    env->DeleteLocalRef(imageData);

}


static JNINativeMethod g_RenderMethods[] = {
        {"native_OnSurfaceCreated", "()V",      (void *) (native_OnSurfaceCreated)},
        {"native_OnSurfaceChanged", "(II)V",    (void *) (native_OnSurfaceChanged)},
        {"native_OnDrawFrame",      "()V",      (void *) (native_OnDrawFrame)},
        {"native_SetImageData",     "(III[B)V", (void *) (native_SetImageData)},
        {"playMP4",     "(Ljava/lang/String;Landroid/view/Surface;I)V", (void *) (playMP4)}
};

static int RegisterNativeMethods(JNIEnv *env, const char *className, JNINativeMethod *methods, int methodNum)
{
    LOGCATE("RegisterNativeMethods");
    jclass clazz = env->FindClass(className);
    if (clazz == NULL)
    {
        LOGCATE("RegisterNativeMethods fail. clazz == NULL");
        return JNI_FALSE;
    }
    if (env->RegisterNatives(clazz, methods, methodNum) < 0)
    {
        LOGCATE("RegisterNativeMethods fail");
        return JNI_FALSE;
    }
    return JNI_TRUE;
}

static void UnregisterNativeMethods(JNIEnv *env, const char *className)
{
    LOGCATE("UnregisterNativeMethods");
    jclass clazz = env->FindClass(className);
    if (clazz == NULL)
    {
        LOGCATE("UnregisterNativeMethods fail. clazz == NULL");
        return;
    }
    if (env != NULL)
    {
        env->UnregisterNatives(clazz);
    }
}


// call this func when loading lib
extern "C" jint JNI_OnLoad(JavaVM *jvm, void *p)
{
//    LOGCATE("===== JNI_OnLoad =====");
    jint jniRet = JNI_ERR;
    JNIEnv *env = NULL;
    if (jvm->GetEnv((void **) (&env), JNI_VERSION_1_6) != JNI_OK)
    {
        return jniRet;
    }

    jint regRet = RegisterNativeMethods(env, NATIVE_RENDER_CLASS_, g_RenderMethods,
                                        sizeof(g_RenderMethods) /
                                        sizeof(g_RenderMethods[0]));
    if (regRet != JNI_TRUE)
    {
        return JNI_ERR;
    }
    env -> GetJavaVM(&GLUtilC::mVm);
//    HelloTest().test1();

    return JNI_VERSION_1_6;
}



extern "C" void JNI_OnUnload(JavaVM *jvm, void *p)
{
    JNIEnv *env = NULL;
    if (jvm->GetEnv((void **) (&env), JNI_VERSION_1_6) != JNI_OK)
    {
        return;
    }

    UnregisterNativeMethods(env, NATIVE_RENDER_CLASS_);

}

#ifdef __cplusplus
}
#endif
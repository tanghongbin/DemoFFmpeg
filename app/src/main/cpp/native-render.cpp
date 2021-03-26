//
// Created by Admin on 2020/8/21.
//


#include <jni.h>
#include <android/log.h>
#include <EGLRender.h>
#include <VideoRender.h>
#include <PlayMp4Instance.h>
#include <encode/EncodeYuvToJpg.h>
#include <EglContext.h>
#include "utils/helpers/JavaVmManager.h"
#include "utils/utils.h"
#include "test/MyGLRenderContext.h"
#include "utils/CustomGLUtils.h"
//com.example.democ.opengles
#define NATIVE_RENDER_CLASS_NAME "com/example/democ/opengles/NativeRender"





#ifdef __cplusplus
extern "C" {
#endif

#include <libavcodec/avcodec.h>

/*
 * Class:     com_byteflow_app_MyNativeRender
 * Method:    native_OnInit
 * Signature: ()V
 */
JNIEXPORT void JNICALL native_OnInit(JNIEnv *env, jobject instance) {
    MyGLRenderContext::GetInstance();
}

/*
 * Class:     com_byteflow_app_MyNativeRender
 * Method:    native_OnUnInit
 * Signature: ()V
 */
JNIEXPORT void JNICALL native_OnUnInit(JNIEnv *env, jobject instance) {
    MyGLRenderContext::GetInstance() -> DestroyInstance();
}

/*
 * Class:     com_byteflow_app_MyNativeRender
 * Method:    native_SetImageData
 * Signature: (III[B)V
 */
JNIEXPORT void JNICALL native_SetImageData
        (JNIEnv *env, jobject instance, jint format, jint width, jint height,
         jbyteArray imageData) {
    int len = env->GetArrayLength(imageData);
    uint8_t *buf = new uint8_t[len];
    env->GetByteArrayRegion(imageData, 0, len, reinterpret_cast<jbyte *>(buf));
    MyGLRenderContext::GetInstance()->SetImageData(format, width, height, buf);
    delete[] buf;
    env->DeleteLocalRef(imageData);

}

/*
 * Class:     com_byteflow_app_MyNativeRender
 * Method:    native_OnSurfaceCreated
 * Signature: ()V
 */
JNIEXPORT void JNICALL native_OnSurfaceCreated(JNIEnv *env, jobject instance,
        jint sampleType,jstring vertexStr,jstring fragStr) {
    MyGLRenderContext::GetInstance() -> OnSurfaceCreated(sampleType,getCharStrFromJstring(env,vertexStr),
            getCharStrFromJstring(env,fragStr));
}

/*
 * Class:     com_byteflow_app_MyNativeRender
 * Method:    native_OnSurfaceCreated
 * Signature: ()V
 */
JNIEXPORT jstring JNICALL getAvCodeInfo(JNIEnv *env, jobject instance) {
    std::string result = avcodec_configuration();
    return env -> NewStringUTF(result.c_str());
}

/*
 * Class:     com_byteflow_app_MyNativeRender
 * Method:    native_OnSurfaceCreated
 * Signature: ()V
 */
JNIEXPORT jint JNICALL native_getVideoWidth(JNIEnv *env, jobject instance) {
    return VideoRender::m_RenderWidth;
}

/*
 * Class:     com_byteflow_app_MyNativeRender
 * Method:    native_OnSurfaceCreated
 * Signature: ()V
 */
JNIEXPORT jint JNICALL native_getVideoHeight(JNIEnv *env, jobject instance) {
    return VideoRender::m_RenderHeight;
}

JNIEXPORT jstring JNICALL encodeYuvToImage(JNIEnv *env, jobject instance,jstring url) {
    const char * result = env->GetStringUTFChars(url, 0);
    std::string encodedPath = EncodeYuvToJpg::encode(result);
    return env -> NewStringUTF(encodedPath.c_str());
}

JNIEXPORT void JNICALL testThread(JNIEnv *env, jobject instance) {

    testLocalThread();

}

PlayMp4Instance* playMp4Instance;

/**
 * 播放本地MP4文件
 * @param env
 * @param instance
 */
JNIEXPORT void JNICALL playMP4(JNIEnv *env, jobject instance,jstring url,jobject surface) {
    LOGCATE("prepare play mp4");
    if (playMp4Instance != nullptr){
        playMp4Instance->unInit();
        delete playMp4Instance;
        playMp4Instance = nullptr;
    }
    playMp4Instance = new PlayMp4Instance();
    const char * playUrl = env->GetStringUTFChars(url,0);
    LOGCATE("prepare init mp4 , detected address %s",playUrl);
    playMp4Instance->init(playUrl,env,instance,surface,2);
}


/*
 * Class:     com_byteflow_app_MyNativeRender
 * Method:    native_OnSurfaceChanged
 * Signature: (II)V
 */
JNIEXPORT void JNICALL native_OnSurfaceChanged
        (JNIEnv *env, jobject instance, jint width, jint height) {
    MyGLRenderContext::GetInstance() -> OnSurfaceChanged(width,height);
}

/*
 * Class:     com_byteflow_app_MyNativeRender
 * Method:    native_OnSurfaceChanged
 * Signature: (II)V
 */
JNIEXPORT void JNICALL native_seekPosition
        (JNIEnv *env, jobject instance, jint position) {
    playMp4Instance->seekPosition(position);
}

JNIEXPORT jlong JNICALL native_getTotalDuration
        (JNIEnv *env, jobject instance, jint position) {
    return playMp4Instance->totalDuration;
}


/*
 * Class:     com_byteflow_app_MyNativeRender
 * Method:    native_OnDrawFrame
 * Signature: ()V
 */
JNIEXPORT void JNICALL native_OnDrawFrame(JNIEnv *env, jobject instance) {

    MyGLRenderContext::GetInstance() -> OnDrawFrame();
}

JNIEXPORT void JNICALL native_eglInit(JNIEnv *env, jobject instance) {
    // 0- 正常
    // 1- 网格
    // 2- 马赛克
    // 3- 网格
    // 4- 形变
    // 5- 边缘
    // 6- 放大
    // 7- 形变
    int type = 4;
    EGLRender::GetInstance() -> Init(type);
}

JNIEXPORT void JNICALL native_eglDraw(JNIEnv *env, jobject instance) {
    EGLRender::GetInstance() -> Draw();

}

JNIEXPORT void JNICALL native_eglUnInit(JNIEnv *env, jobject instance) {
    EGLRender::GetInstance() -> UnInit();

}

JNIEXPORT void JNICALL native_pause(JNIEnv *env, jobject instance) {
    playMp4Instance->pauseManual();

}

JNIEXPORT void JNICALL native_resume(JNIEnv *env, jobject instance) {
    playMp4Instance->resume();
}

JNIEXPORT void JNICALL native_eglSetImageData(JNIEnv *env, jobject instance,  jint width, jint height,
                                              jbyteArray imageData) {
    int len = env->GetArrayLength(imageData);
    uint8_t *buf = new uint8_t[len];
    env->GetByteArrayRegion(imageData, 0, len, reinterpret_cast<jbyte *>(buf));
    EGLRender::GetInstance() ->SetImageData(buf,width,height);
    delete[] buf;
    env->DeleteLocalRef(imageData);
}

JNIEXPORT void JNICALL egl_init_2(JNIEnv *env, jobject instance) {
    EglContext::init();
}




static JNINativeMethod g_RenderMethods[] = {
        {"native_OnInit",           "()V",      (void *) (native_OnInit)},
        {"native_OnUnInit",         "()V",      (void *) (native_OnUnInit)},
        {"native_SetImageData",     "(III[B)V", (void *) (native_SetImageData)},
        {"native_OnSurfaceCreated", "(Ljava/lang/String;Ljava/lang/String;I)V",      (void *) (native_OnSurfaceCreated)},
        {"native_OnSurfaceChanged", "(II)V",    (void *) (native_OnSurfaceChanged)},
        {"native_OnDrawFrame",      "()V",      (void *) (native_OnDrawFrame)},
        {"native_getVideoWidth",      "()I",      (void *) (native_getVideoWidth)},
        {"native_getVideoHeight",      "()I",      (void *) (native_getVideoHeight)},
        {"native_getTotalDuration",      "()J",      (void *) (native_getTotalDuration)},
        {"native_eglInit",      "()V",      (void *) (native_eglInit)},
        {"native_pause",      "()V",      (void *) (native_pause)},
        {"native_resume",      "()V",      (void *) (native_resume)},
        {"testThread",      "()V",      (void *) (testThread)},
        {"native_seekPosition",      "(I)V",      (void *) (native_seekPosition)},
        {"native_eglDraw",      "()V",      (void *) (native_eglDraw)},
        {"native_eglUnInit",      "()V",      (void *) (native_eglUnInit)},
        {"native_eglSetImageData",     "(II[B)V", (void *) (native_eglSetImageData)},
        {"getAvCodeInfo",     "()Ljava/lang/String;", (void *) (getAvCodeInfo)},
        {"encodeYuvToImage",     "(Ljava/lang/String;)Ljava/lang/String;", (void *) (encodeYuvToImage)},
        {"playMP4",     "(Ljava/lang/String;Landroid/view/Surface;)V", (void *) (playMP4)},

         // ================  egl ctx ==================
                {"egl_init_2",      "()V",      (void *) (egl_init_2)}

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

    jint regRet = RegisterNativeMethods(env, NATIVE_RENDER_CLASS_NAME, g_RenderMethods,
                                        sizeof(g_RenderMethods) /
                                        sizeof(g_RenderMethods[0]));
    if (regRet != JNI_TRUE)
    {
        return JNI_ERR;
    }
    JavaVmManager::initVm(env);
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

    UnregisterNativeMethods(env, NATIVE_RENDER_CLASS_NAME);

}

#ifdef __cplusplus
}
#endif



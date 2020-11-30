//
// Created by Admin on 2020/9/27.
//


// 测试ffmpeg的编解码，格式转换，混合和解混合，推拉流

#include <jni.h>
#include <CustomGLUtils.h>
#include <YuvToImageRender.h>
#include <OpenGLFFmpegRender.h>
#include <PlayMp4Instance.h>
#include <VideoGLRender.h>
#include <PlayMp4Practice.h>
#include <helpers/JavaVmManager.h>
#include <capturer/AudioRecordPlayHelper.h>
#include <encode/FFmpegEncodeAudio.h>
#include <encode/EncodeYuvToJpg.h>
#include <encode/FFmpegEncodeVideo.h>
#include <encode/SwsConvertYuvToPng.h>
#include <GLES3/gl3.h>
#include <filters/WaterFilterHelper.h>
#include <filters/EncodeYuvToYuvByFilter.h>
#include <swscale/SwsConvertYuvToRgb.h>
#include <swscale/ConvertMp4ToFlv.h>
#include <muxer/CustomDemuxer.h>
#include <muxer/CustomMuxer.h>
#include <SingleNodeList.h>
#include <TimeTracker.h>


#define NATIVE_RENDER_CLASS_ "com/example/democ/render/FFmpegRender"


void addNum(SingleNodeList *nodeList);

void addNum(SingleNodeList *nodeList) {
    const char *array[12] = {"a", "b", "c", "d", "e", "f", "g", "h", "i", "j"};
    for (int i = 0; i < 500; ++i) {
        int index = i % 10;
        const char *tem = array[index];
        CustomNode *newNode = new CustomNode;
        newNode->point = tem;
        nodeList->pushLast(newNode);
    }
}

#ifdef __cplusplus


extern "C" {
#endif

#include <libavutil/imgutils.h>
#include <libavcodec/jni.h>

/*
 * Class:     com_byteflow_app_MyNativeRender
 * Method:    native_OnSurfaceCreated
 * Signature: ()V
 */



JNIEXPORT void JNICALL native_OnSurfaceCreated(JNIEnv *env, jobject instance) {
//    VideoGLRender::GetInstance()->OnSurfaceCreated();
}

/*
 * Class:     com_byteflow_app_MyNativeRender
 * Method:    native_OnSurfaceChanged
 * Signature: (II)V
 */
JNIEXPORT void JNICALL native_OnSurfaceChanged
        (JNIEnv *env, jobject instance, jint width, jint height) {
//    VideoGLRender::GetInstance()->OnSurfaceChanged(width, height);
// 正常情况下这样设置
//    FFmpegEncodeVideo::getInstance()->mWindow_width = height;
//    FFmpegEncodeVideo::getInstance()->mWindow_height = width;

    FFmpegEncodeVideo::getInstance()->mWindow_width = width;
    FFmpegEncodeVideo::getInstance()->mWindow_height = height;
    LOGCATE("setup width:%d height:%d", FFmpegEncodeVideo::getInstance()->mWindow_width,
            FFmpegEncodeVideo::getInstance()->mWindow_height);
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
//    VideoGLRender::GetInstance()->OnDrawFrame();
}

JNIEXPORT void JNICALL native_startEncode(JNIEnv *env, jobject instance) {
    FFmpegEncodeAudio::getInstance()->init();
//    FFmpegEncodeAudio::getInstance()->initOffcialDemo();
}

JNIEXPORT void JNICALL native_yuv2rgb(JNIEnv *env, jobject instance) {
    SwsConvertYuvToRgb::convert();
}

JNIEXPORT jstring JNICALL
native_changeOutputFormat(JNIEnv *env, jobject instance, jstring jstring1) {
    ConvertMp4ToFlv::convert(getCharStrFromJstring(env, jstring1), ".flv");
    ConvertMp4ToFlv::convert(getCharStrFromJstring(env, jstring1), ".mkv");
    ConvertMp4ToFlv::convert(getCharStrFromJstring(env, jstring1), ".mp4");
    ConvertMp4ToFlv::convert(getCharStrFromJstring(env, jstring1), ".avi");
    ConvertMp4ToFlv::convert(getCharStrFromJstring(env, jstring1), ".wvm");
    return getJstringFromCharStr(env, "");
}


JNIEXPORT jstring JNICALL native_addFilterToYuv(JNIEnv *env, jobject instance, jstring inName) {
    EncodeYuvToYuvByFilter *filterHelper = new EncodeYuvToYuvByFilter;
    const char *result = filterHelper->yuvToyuvByFilter(getCharStrFromJstring(env, inName));
    delete filterHelper;
    return getJstringFromCharStr(env, result);
//    FFmpegEncodeAudio::getInstance()->initOffcialDemo();
}

JNIEXPORT jstring JNICALL encodeYuvToImage(JNIEnv *env, jobject instance, jstring jstring1) {
    return getJstringFromCharStr(env, EncodeYuvToJpg::encode(getCharStrFromJstring(env, jstring1)));
}

//JNIEXPORT void JNICALL swsPng(JNIEnv *env, jobject instance) {
//    SwsConvertYuvToPng::encode("");
//}

JNIEXPORT void JNICALL native_audioTest(JNIEnv *env, jobject instance, jint type) {
    switch (type) {
        case 1:
            FFmpegEncodeAudio::getInstance();
            AudioRecordPlayHelper::getInstance()->startCapture();
            break;
        case 2:
            AudioRecordPlayHelper::getInstance()->stopCapture();
            break;
        case 3:
            AudioRecordPlayHelper::getInstance()->startPlayBack();
            break;
        case 4:
            AudioRecordPlayHelper::getInstance()->stopPlayBack();
            break;
        default:
            break;
    }
}

PlayMp4Instance *playMp4Instance;

PlayMp4Practice *playMp4Practice;

JNIEXPORT void JNICALL native_unInit(JNIEnv *env, jobject instance) {
    AudioRecordPlayHelper::destroyInstance();
    FFmpegEncodeAudio::destroyInstance();
    if (playMp4Instance) {
        delete playMp4Instance;
        playMp4Instance = nullptr;
    }
    if (playMp4Practice) {
        playMp4Practice->stopPlay();
        delete playMp4Practice;
        playMp4Practice = nullptr;
    }
}



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


JNIEXPORT void JNICALL playMP4(JNIEnv *env, jobject instance, jstring url, jobject surface) {
    LOGCATE("prepare play mp4");
    playMp4Practice = new PlayMp4Practice();
    const char *playUrl = env->GetStringUTFChars(url, 0);
    LOGCATE("prepare init mp4 , detected address %s", playUrl);
    playMp4Practice->init(playUrl, env, instance, surface, 1);
    playMp4Practice->init(playUrl, env, instance, surface, 2);
}

JNIEXPORT void JNICALL native_encodeFrame(JNIEnv *env, jobject instance,
                                          jbyteArray imageData) {

    jbyte *data = env->GetByteArrayElements(imageData, 0);

    FFmpegEncodeVideo::getInstance()->encodeVideoFrame(reinterpret_cast<uint8_t *>(data));

    env->ReleaseByteArrayElements(imageData, data, 0);


//    int len = env->GetArrayLength(imageData);
//    LOGCATE("log jbytearray length:%d",len);
//    uint8_t *buf = new uint8_t[len];
//    env->GetByteArrayRegion(imageData, 0, len, reinterpret_cast<jbyte *>(buf));
//
//    delete[] buf;
//    env->DeleteLocalRef(imageData);
}

JNIEXPORT void JNICALL native_videoEncodeInit(JNIEnv *env, jobject instance) {
    FFmpegEncodeVideo::getInstance()->init();
//    FFmpegEncodeVideo::getInstance() -> initOffcialDemo();
}

JNIEXPORT void JNICALL native_videoEncodeUnInit(JNIEnv *env, jobject instance) {
    FFmpegEncodeVideo::getInstance()->unInit();
    FFmpegEncodeVideo::destroyInstance();
}

SingleNodeList *listHelper;

JNIEXPORT void JNICALL native_testReadFile(JNIEnv *env, jobject instance) {
    if (!listHelper) listHelper = new SingleNodeList;

    TimeTracker::trackBegin();
    std::thread *thread1 = new std::thread(addNum, listHelper);
    std::thread *thread2 = new std::thread(addNum, listHelper);

    thread1->join();
    thread2->join();

    int start = 1;
    while (listHelper->popFirst()) {
        start++;
    }
    TimeTracker::trackOver();
    LOGCATE("this is times:%d", start);
    if (listHelper) {
        delete listHelper;
        listHelper = nullptr;
    }

}


JNIEXPORT void JNICALL native_muxerAudioAndVideo(JNIEnv *env, jobject instance) {
    long long start = GetSysCurrentTime();
    CustomMuxer::muxer();
    LOGCATE("total cost:%lld", GetSysCurrentTime() - start);
}


JNIEXPORT void JNICALL splitAudioAndVideo(JNIEnv *env, jobject instance, jstring path) {
    CustomDemuxer::demuxerDiffcult(getCharStrFromJstring(env, path));
}


static int
RegisterNativeMethods(JNIEnv *env, const char *className, JNINativeMethod *methods, int methodNum) {
    LOGCATE("RegisterNativeMethods");
    jclass clazz = env->FindClass(className);
    if (clazz == NULL) {
        LOGCATE("RegisterNativeMethods fail. clazz == NULL");
        return JNI_FALSE;
    }
    if (env->RegisterNatives(clazz, methods, methodNum) < 0) {
        LOGCATE("RegisterNativeMethods fail");
        return JNI_FALSE;
    }
    return JNI_TRUE;
}

static void UnregisterNativeMethods(JNIEnv *env, const char *className) {
    LOGCATE("UnregisterNativeMethods");
    jclass clazz = env->FindClass(className);
    if (clazz == NULL) {
        LOGCATE("UnregisterNativeMethods fail. clazz == NULL");
        return;
    }
    if (env != NULL) {
        env->UnregisterNatives(clazz);
    }
}

static JNINativeMethod g_RenderMethods[] = {
        {"native_OnSurfaceCreated",   "()V",                                         (void *) (native_OnSurfaceCreated)},
        {"native_OnSurfaceChanged",   "(II)V",                                       (void *) (native_OnSurfaceChanged)},
        {"native_OnDrawFrame",        "()V",                                         (void *) (native_OnDrawFrame)},

        {"native_videoEncodeInit",    "()V",                                         (void *) (native_videoEncodeInit)},
        {"native_videoEncodeUnInit",  "()V",                                         (void *) (native_videoEncodeUnInit)},
        {"native_testReadFile",       "()V",                                         (void *) (native_testReadFile)},
        {"splitAudioAndVideo",        "(Ljava/lang/String;)V",                       (void *) (splitAudioAndVideo)},
        {"native_changeOutputFormat", "(Ljava/lang/String;)Ljava/lang/String;",      (void *) (native_changeOutputFormat)},

        {"native_muxerAudioAndVideo", "()V",                                         (void *) (native_muxerAudioAndVideo)},
        {"native_startEncode",        "()V",                                         (void *) (native_startEncode)},
        {"native_encodeFrame",        "([B)V",                                       (void *) (native_encodeFrame)},
        {"encodeYuvToImage",          "(Ljava/lang/String;)Ljava/lang/String;",      (void *) (encodeYuvToImage)},
        {"native_addFilterToYuv",     "(Ljava/lang/String;)Ljava/lang/String;",      (void *) (native_addFilterToYuv)},

        {"native_audioTest",          "(I)V",                                        (void *) (native_audioTest)},
        {"native_unInit",             "()V",                                         (void *) (native_unInit)},
        {"native_yuv2rgb",            "()V",                                         (void *) (native_yuv2rgb)},


        {"playMP4",                   "(Ljava/lang/String;Landroid/view/Surface;)V", (void *) (playMP4)}
};


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

//    HelloTest().test1();

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
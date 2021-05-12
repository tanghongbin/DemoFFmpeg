//
// Created by Admin on 2020/9/27.
//


// 测试ffmpeg的编解码，格式转换，混合和解混合，推拉流

#include <jni.h>
#include <CustomGLUtils.h>
#include <YuvToImageRender.h>
#include <OpenGLFFmpegRender.h>
#include <Mp4Demo.h>
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
#include <TimeTracker.h>
#include <encode/FFmpegEncodeAVToMp4.h>
#include <encode/FFmpegEncodeAVToLiveRtmp.h>
#include <rtmp.h>
#include <rtmp/RtmpClient.h>
#include <curl/curl.h>


#define NATIVE_RENDER_CLASS_ "com/example/democ/render/FFmpegRender"


//void addNum(SingleNodeList<const char *> *nodeList);
//
//void addNum(SingleNodeList<const char *> *nodeList) {
//    const char *array[12] = {"a", "b", "c", "d", "e", "f", "g", "h", "i", "j"};
//    for (int i = 0; i < 500; ++i) {
//        int index = i % 10;
//        const char *tem = array[index];
//        nodeList->pushLast(tem);
//    }
//}

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
    VideoGLRender::GetInstance()->OnSurfaceCreated();
}

/*
 * Class:     com_byteflow_app_MyNativeRender
 * Method:    native_OnSurfaceChanged
 * Signature: (II)V
 */
JNIEXPORT void JNICALL native_OnSurfaceChanged
        (JNIEnv *env, jobject instance, jint width, jint height) {
    VideoGLRender::GetInstance()->OnSurfaceChanged(width, height);
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
    if (VideoGLRender::checkInstanceExist()){
        VideoGLRender::GetInstance()->OnDrawFrame();
    }
}

JNIEXPORT void JNICALL native_startEncode(JNIEnv *env, jobject instance,jstring jstring1) {
    FFmpegEncodeAudio::getInstance()->init(getCharStrFromJstring(env,jstring1));
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

JNIEXPORT jstring JNICALL native_getSimpleInfo(JNIEnv *env, jobject instance) {
    return getJstringFromCharStr(env,avformat_configuration());
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

Mp4Demo *mp4Demo;

PlayMp4Practice *playMp4Practice;

JNIEXPORT void JNICALL native_unInit(JNIEnv *env, jobject instance) {
    AudioRecordPlayHelper::destroyInstance();
    FFmpegEncodeAudio::destroyInstance();
    if (mp4Demo) {
        delete mp4Demo;
        mp4Demo = nullptr;
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
//    if (mp4Demo != nullptr){
//        mp4Demo->unInit();
//        delete mp4Demo;
//        mp4Demo = nullptr;
//    }
//    mp4Demo = new Mp4Demo();
//    const char * playUrl = env->GetStringUTFChars(url,0);
//    LOGCATE("prepare init mp4 , detected address %s",playUrl);
//    mp4Demo->init(playUrl,env,instance,surface,type);
//}

JNIEXPORT void JNICALL native_seekPosition
        (JNIEnv *env, jobject instance, jint position) {
    playMp4Practice->seekPosition(position);
}


JNIEXPORT void JNICALL playMP4(JNIEnv *env, jobject instance, jstring url, jobject surface) {
    playMp4Practice = new PlayMp4Practice();
    const char *playUrl = env->GetStringUTFChars(url, 0);
    // 1-音频，2-nativewindow，4-opengles_window
    int type = surface != nullptr ? 2 : 4;
    playMp4Practice->init(playUrl, env, instance, surface, 1);
    playMp4Practice->init(playUrl, env, instance, surface, type);
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

//SingleNodeList< const char *> *listHelper;

size_t print_html( void *ptr, size_t size, size_t nmemb, void *stream)
{
    int len = size*nmemb;
    char *buf = new char[len+1];

    memcpy(buf, ptr, len);

    buf[len] = '\0';

    LOGCATE("success result:%s\n", buf);

    delete [] buf;

    return len;
}

void request(){
    CURL * curl = NULL;
    CURLcode  ret;
    curl = curl_easy_init();
    const char * url = "www.baidu.com";
    curl_easy_setopt(curl,CURLOPT_URL,url);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, print_html);
    curl_easy_setopt(curl, CURLOPT_VERBOSE, 1); /* 打开debug */
//    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, "Authorization: eyJ0IjoiMSIsInYiOiIxIiwiYiI6IjEifQ.Dc2FzJsp36Zypd1da7SosFly0IZVYv1QfuZPkZXZdA8OHM3fZHo5fc6JwURRqOdlOBuoizMg0ibmKR1IKYGkYHG5nr55LMoS.ojmQpiLs9NYE0Uzf6EF9qsrHm7P3DG92nboOWUE-PSE");

    ret = curl_easy_perform(curl);
    if (ret != CURLE_OK) {
        LOGCATE("curl_easy_perform error");
        return ;
    }

    curl_easy_cleanup(curl);
}

JNIEXPORT void JNICALL native_testReadFile(JNIEnv *env, jobject instance) {

    std::thread* thread = new std::thread(request);
    thread->join();
    delete thread;
}


JNIEXPORT void JNICALL native_muxerAudioAndVideo(JNIEnv *env, jobject instance) {
    long long start = GetSysCurrentTime();
    CustomMuxer::muxer();
    LOGCATE("total cost:%lld", GetSysCurrentTime() - start);
}


JNIEXPORT void JNICALL splitAudioAndVideo(JNIEnv *env, jobject instance, jstring path) {
    CustomDemuxer::demuxerDiffcult(getCharStrFromJstring(env, path));
}

JNIEXPORT void JNICALL native_startrecordmp4(JNIEnv *env, jobject instance) {
    FFmpegEncodeAVToMp4::getInstance()->startRecordMp4();
}


JNIEXPORT void JNICALL native_stoprecordmp4(JNIEnv *env, jobject instance) {
    FFmpegEncodeAVToMp4::getInstance()->stopRecordMp4();
}


JNIEXPORT void JNICALL native_encodeavmuxer_OnSurfaceCreated(JNIEnv *env, jobject instance) {

}

/*
 * Class:     com_byteflow_app_MyNativeRender
 * Method:    native_OnSurfaceChanged
 * Signature: (II)V
 */
JNIEXPORT void JNICALL native_encodeavmuxer_OnSurfaceChanged
        (JNIEnv *env, jobject instance, jint width, jint height) {

    FFmpegEncodeAVToMp4::getInstance()->mWindow_width = width;
    FFmpegEncodeAVToMp4::getInstance()->mWindow_height = height;
    LOGCATE("setup width:%d height:%d", FFmpegEncodeAVToMp4::getInstance()->mWindow_width,
            FFmpegEncodeAVToMp4::getInstance()->mWindow_height);
    FFmpegEncodeAVToMp4::getInstance()->initAvEncoder();

}

JNIEXPORT void JNICALL native_encodeavmuxer_encodeFrame(JNIEnv *env, jobject instance,
                                                        jbyteArray imageData) {

    jbyte *data = env->GetByteArrayElements(imageData, 0);

    FFmpegEncodeAVToMp4::getInstance()->encode_frame_Av(reinterpret_cast<uint8_t *>(data), 0, 2);

    env->ReleaseByteArrayElements(imageData, data, 0);

}

JNIEXPORT jstring JNICALL native_unInitRecordMp4(JNIEnv *env, jobject instance) {
    jstring result = getJstringFromCharStr(env, FFmpegEncodeAVToMp4::getInstance()->out_file_name);
    FFmpegEncodeAVToMp4::getInstance()->unInitAvEncoder();
    FFmpegEncodeAVToMp4::destroyInstance();
    return result;
}


/// ==============================   直播 ===================

JNIEXPORT void JNICALL native_live_startpush(JNIEnv *env, jobject instance) {
    FFmpegEncodeAVToLiveRtmp::getInstance()->startRecordMp4();
}

JNIEXPORT void JNICALL native_live_stoppush(JNIEnv *env, jobject instance) {
    FFmpegEncodeAVToLiveRtmp::getInstance()->stopRecordMp4();
}

JNIEXPORT void JNICALL native_live_onDestroy(JNIEnv *env, jobject instance) {
    FFmpegEncodeAVToLiveRtmp::getInstance()->unInitAvEncoder();
    FFmpegEncodeAVToLiveRtmp::destroyInstance();
}

JNIEXPORT void JNICALL native_live_OnSurfaceCreated(JNIEnv *env, jobject instance) {

}

/*
 * Class:     com_byteflow_app_MyNativeRender
 * Method:    native_OnSurfaceChanged
 * Signature: (II)V
 */
JNIEXPORT void JNICALL native_live__OnSurfaceChanged
        (JNIEnv *env, jobject instance, jint width, jint height) {

    FFmpegEncodeAVToLiveRtmp::getInstance()->mWindow_width = width;
    FFmpegEncodeAVToLiveRtmp::getInstance()->mWindow_height = height;
    LOGCATE("setup width:%d height:%d", FFmpegEncodeAVToLiveRtmp::getInstance()->mWindow_width,
            FFmpegEncodeAVToLiveRtmp::getInstance()->mWindow_height);
    FFmpegEncodeAVToLiveRtmp::getInstance()->initAvEncoder();

}

JNIEXPORT void JNICALL native_live_encodeFrame(JNIEnv *env, jobject instance,
                                                        jbyteArray imageData) {

    jbyte *data = env->GetByteArrayElements(imageData, 0);

    FFmpegEncodeAVToLiveRtmp::getInstance()->encode_frame_Av(reinterpret_cast<uint8_t *>(data), 0, 2);

    env->ReleaseByteArrayElements(imageData, data, 0);

}


/****
 *  ======================================== rtmp 推流 ======================================
 */

JNIEXPORT void JNICALL native_sendPacketData(JNIEnv *env, jobject instance,
                                              jbyteArray byteData,jint type) {
    int length = env->GetArrayLength(byteData);

//    LOGCATE("log data size:%d",length);

    jbyte *data = env->GetByteArrayElements(byteData, 0);

    RtmpClient::getInstance()->sendData(reinterpret_cast<uint8_t *>(data),type,length);

    env->ReleaseByteArrayElements(byteData, data, 0);
//    LOGCATE("jni has receive data:%p  type:%d",byteData,type);

}

JNIEXPORT void JNICALL native_rtmp_init(JNIEnv *env, jobject instance,jstring url) {
    RtmpClient::destroyInstance();
    char * result = const_cast<char *>(getCharStrFromJstring(env, url));
    RtmpClient::getInstance()->initRtmp(result);
}

JNIEXPORT void JNICALL native_rtmp_destroy(JNIEnv *env, jobject instance) {
    RtmpClient::getInstance()->destroyRtmp();
    RtmpClient::getInstance()->destroyInstance();
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
        {"native_OnSurfaceCreated",               "()V",                                         (void *) (native_OnSurfaceCreated)},
        {"native_OnSurfaceChanged",               "(II)V",                                       (void *) (native_OnSurfaceChanged)},
        {"native_OnDrawFrame",                    "()V",                                         (void *) (native_OnDrawFrame)},

        {"native_videoEncodeInit",                "()V",                                         (void *) (native_videoEncodeInit)},
        {"native_videoEncodeUnInit",              "()V",                                         (void *) (native_videoEncodeUnInit)},
        {"native_testReadFile",                   "()V",                                         (void *) (native_testReadFile)},
        {"splitAudioAndVideo",                    "(Ljava/lang/String;)V",                       (void *) (splitAudioAndVideo)},
        {"native_changeOutputFormat",             "(Ljava/lang/String;)Ljava/lang/String;",
                                                                                                 (void *) (native_changeOutputFormat)},

        {"native_muxerAudioAndVideo",             "()V",                                         (void *) (native_muxerAudioAndVideo)},
        {"native_startEncode",                    "(Ljava/lang/String;)V",                                         (void *) (native_startEncode)},
        {"native_encodeFrame",                    "([B)V",                                       (void *) (native_encodeFrame)},
        {"encodeYuvToImage",                      "(Ljava/lang/String;)Ljava/lang/String;",      (void *) (encodeYuvToImage)},
        {"native_addFilterToYuv",                 "(Ljava/lang/String;)Ljava/lang/String;",      (void *) (native_addFilterToYuv)},
        {"native_getSimpleInfo",                  "()Ljava/lang/String;",                        (void *) (native_getSimpleInfo)},


        {"native_audioTest",                      "(I)V",                                        (void *) (native_audioTest)},
        {"native_unInit",                         "()V",                                         (void *) (native_unInit)},
        {"native_yuv2rgb",                        "()V",                                         (void *) (native_yuv2rgb)},
        {"native_startrecordmp4",                 "()V",                                         (void *) (native_startrecordmp4)},
        {"native_stoprecordmp4",                  "()V",                                         (void *) (native_stoprecordmp4)},

        {"native_unInitRecordMp4",                "()Ljava/lang/String;",                        (void *) (native_unInitRecordMp4)},
        {"playMP4",                               "(Ljava/lang/String;Landroid/view/Surface;)V", (void *) (playMP4)},
        {"native_encodeavmuxer_OnSurfaceCreated", "()V",                                         (void *) (native_encodeavmuxer_OnSurfaceCreated)},
        {"native_encodeavmuxer_OnSurfaceChanged", "(II)V",                                       (void *) (native_encodeavmuxer_OnSurfaceChanged)},
        {"native_encodeavmuxer_encodeFrame",      "([B)V",                                       (void *) (native_encodeavmuxer_encodeFrame)},
//        {"getVideoDimensions",      "()[I",                                       (void *) (getVideoDimensions)},
        {"native_seekPosition",      "(I)V",      (void *) (native_seekPosition)},

        /**
         * ================================    直播模块 =====================================
         */
        {"native_live_OnSurfaceCreated", "()V",                                         (void *) (native_live_OnSurfaceCreated)},
        {"native_live__OnSurfaceChanged", "(II)V",                                       (void *) (native_live__OnSurfaceChanged)},
        {"native_live_encodeFrame",      "([B)V",                                       (void *) (native_live_encodeFrame)},
        {"native_live_startpush",                    "()V",                                       (void *) (native_live_startpush)},
        {"native_live_stoppush",                    "()V",                                       (void *) (native_live_stoppush)},
        {"native_live_onDestroy",                    "()V",                                       (void *) (native_live_onDestroy)},

        /****
         * ================================= rtmp 推流
         */
        {"native_sendPacketData",      "([BI)V",                                       (void *) (native_sendPacketData)},
        {"native_rtmp_init",                    "(Ljava/lang/String;)V",                                       (void *) (native_rtmp_init)},
        {"native_rtmp_destroy",                    "()V",                                       (void *) (native_rtmp_destroy)}

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
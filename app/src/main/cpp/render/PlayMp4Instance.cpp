//
// Created by Admin on 2020/10/10.
//

#include <CustomGLUtils.h>
#include <helpers/JavaVmManager.h>
#include "PlayMp4Instance.h"
#include "VideoRender.h"
#include "BaseRender.h"
#include "AudioRender.h"
#include "YuvToImageRender.h"
#include "OpenGLFFmpegDecoder.h"

jobject PlayMp4Instance::mNativeRender = nullptr;
long PlayMp4Instance::totalDuration = 0;


void PlayMp4Instance::init(const char *url, JNIEnv *jniEnv, jobject nativeRender, jobject surface,int videoType) {
    LOGCATE("init has started");
    strcpy(mPlayUrl, url);
    LOGCATE("init has reached address playUrl:%s", mPlayUrl);
    LOGCATE("init has reached vm success");
    mNativeRender = jniEnv->NewGlobalRef(nativeRender);
    mSurfaceInstance = jniEnv->NewGlobalRef(surface);
    LOGCATE("prepare created short thread");
    audio = new thread(createThreadForPlay, this, mNativeRender, url, mSurfaceInstance, 1);
    video = new thread(createThreadForPlay, this, mNativeRender, url, mSurfaceInstance, videoType);
    LOGCATE("thread has started");
}

void PlayMp4Instance::unInit() {
    bool isAttach = false;
    JNIEnv *jniEnv = JavaVmManager::GetEnv(&isAttach);
    jniEnv->DeleteGlobalRef(mNativeRender);
    jniEnv->DeleteGlobalRef(mSurfaceInstance);
    if (isAttach) JavaVmManager::detachCurrentThread();
    if (audio) {
        delete audio;
        audio = nullptr;
    }
    if (video) {
        delete video;
        video = nullptr;
    }
}


void PlayMp4Instance::seekPosition(int position) {

    BaseRender::setupSeekPosition(position, 2);
    BaseRender::setupSeekPosition(position, 1);

}




BaseRender *getRender(jint type) {
    // 1-音频，2-视频-nativeWindow，3-视频-opengl
    BaseRender *render;
    switch (type) {
        case 1:
            render = new AudioRender();
            break;
        case 2:
            render = new VideoRender();
            break;
        case 3:
            render = new YuvToImageRender();
            break;
        case 4:
            render = new OpenGLFFmpegDecoder();
            break;
    }
    return render;
}

void
PlayMp4Instance::createThreadForPlay(PlayMp4Instance *selfInstance, _jobject *instance,
                                     const char *localUrl, _jobject *pJobject,
                                     jint type) {

    LOGCATE("播放地址:%s", localUrl);

    time_t t;
    time(&t);
    LOGCATE("打印时间:%d", t);
    // 1-音频，2-视频
    //1.创建封装格式上下文
    AVFormatContext *mFormatContext = avformat_alloc_context();
    //2.打开文件解封装
    if (avformat_open_input(&mFormatContext, localUrl, NULL, NULL) != 0) {
        LOGCATE("init format error");
        return;
    }

    //3.获取音视频流信息
    if (avformat_find_stream_info(mFormatContext, NULL) < 0) {
        LOGCATE("can't find valid stream info");
        return;
    }

    LOGCATE("println all duration:%jd", mFormatContext->duration / AV_TIME_BASE);
    totalDuration = mFormatContext->duration / AV_TIME_BASE;

    int m_StreamIndex = -1;
    //4.获取音视频流索引，先放视频
    AVMediaType avMediaType = type == 1 ? AVMEDIA_TYPE_AUDIO : AVMEDIA_TYPE_VIDEO;
    for (int i = 0; i < mFormatContext->nb_streams; i++) {
        if (mFormatContext->streams[i]->codecpar->codec_type == avMediaType) {
            m_StreamIndex = i;
            break;
        }
    }
    if (m_StreamIndex == -1) {
        LOGCATE("can't find stream index");
        return;
    }

    //5. 根据流找到解码器id,因为流里面有对应的编码信息
    AVCodecParameters *code_params = mFormatContext->streams[m_StreamIndex]->codecpar;
    AVCodec *decoder = avcodec_find_decoder(code_params->codec_id);
    if (decoder == nullptr) {
        LOGCATE("'t find decoder");
        return;
    }

    //6.创建解码器上下文
    AVCodecContext *decode_context = avcodec_alloc_context3(decoder);
    if (avcodec_parameters_to_context(decode_context, code_params) != 0) {
        LOGCATE("avcodec_parameters_to_context failed");
        return;
    }

    // 7.打开解码器
    int openResult = avcodec_open2(decode_context, decoder, NULL);
    if (openResult < 0) {
        LOGCATE("open codec failed : %d", openResult);
        return;
    }

    // 8.创建数据结构体，编码数据和解码数据
    AVPacket *packet = av_packet_alloc();
    AVFrame *frame = av_frame_alloc();

    BaseRender *baseRender = getRender(type);
    baseRender->init(decode_context, instance, pJobject);

    LOGCATE("render has init success ,prepare start decode");

    selfInstance->decodeLoop(selfInstance, mFormatContext, decode_context, packet, frame,
                             baseRender, pJobject, m_StreamIndex);

    //10.解码完成，释放资源
    baseRender->unInit();
    delete baseRender;

    if (packet != nullptr) {
        av_packet_free(&packet);
        packet = nullptr;
    }

    if (frame != nullptr) {
        av_frame_free(&frame);
        frame = nullptr;
    }

    if (decode_context != nullptr) {
        avcodec_close(decode_context);
        avcodec_free_context(&decode_context);
        decode_context = nullptr;
        decoder = nullptr;
    }

    if (mFormatContext != nullptr) {
        avformat_close_input(&mFormatContext);
        avformat_free_context(mFormatContext);
        mFormatContext = nullptr;
    }
}


void PlayMp4Instance::decodeLoop(PlayMp4Instance *pInstance, AVFormatContext *mFormatContext,
                                 AVCodecContext *decode_context,
                                 AVPacket *packet, AVFrame *frame, BaseRender *baseRender,
                                 jobject pJobject, int m_StreamIndex) {
    //9.解码循环

    for (;;) {

        // 先对视频做播放和暂停
        if (m_StreamIndex == 0 && mPlayStatus == PAUSE){
            LOGCATE("has prepare pause streamindex:%d",m_StreamIndex);
            pauseManual();
            std::unique_lock<std::mutex> lock(mMutex);
            signal.wait(lock);
            LOGCATE("lock has relased ,programs is going on");
        }
        if (m_StreamIndex == 0){
//            LOGCATE("i'm decoding video");
        }
        baseRender->seekPositionFunc(m_StreamIndex, mFormatContext, decode_context);

        int result = av_read_frame(mFormatContext, packet);
//        LOGCATE("log result after seek %d",result);
        int frameCount = 0;
        while (result >= 0) { // 读取帧
            if (packet->stream_index == m_StreamIndex) {
                if (avcodec_send_packet(decode_context, packet) != 0) { // 视频解码
                    return;
                }
                baseRender->eachPacket(packet, decode_context);
                while (avcodec_receive_frame(decode_context, frame) == 0) {
//获取到 m_Frame 解码数据，在这里进行格式转换，然后进行渲染，下一节介绍 ANativeWindow 渲染过程
                    baseRender->draw_frame(decode_context, frame, pJobject);
                    frameCount++;
                }
            }
            if (frameCount > 0) goto Exit;
            av_packet_unref(packet);
            result = av_read_frame(mFormatContext, packet);
        }
        Exit:
        av_packet_unref(packet);

        if (result < 0) break;
    }

    LOGCATE("decode done");
}

void PlayMp4Instance::resume() {
    mPlayStatus = RESUME;
    std::unique_lock<std::mutex> lock(mMutex);
    signal.notify_all();
}

void PlayMp4Instance::pauseManual() {
    mPlayStatus = PAUSE;
}



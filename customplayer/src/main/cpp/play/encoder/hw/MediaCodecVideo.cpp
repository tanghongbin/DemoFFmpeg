//
// Created by Admin on 2021/7/16.
//

#include <encoder/hw/MediaCodecVideo.h>
#include <media/NdkMediaCodec.h>
#include <utils/utils.h>
#include <utils/CustomGLUtils.h>
#include <encoder/hw/VideoConfiguration.h>

extern "C" {
#include <libavutil/time.h>
}

void MediaCodecVideo::setOutputDataListener(OutputFmtChangedListener changedListener,OutputDataListener dataListener){
    this->outputFmtChangedListener = changedListener;
    this->outputDataListener = dataListener;
}

void MediaCodecVideo::startEncode(){
    if (isRunning) return;
    mMediaCodec = createVideoMediaCodec();
    if (mMediaCodec == nullptr) {
        return;
    }
    AMediaCodec_start(mMediaCodec);
    encodeThread = new std::thread(loopEncode, this);
    isRunning = true;
}

void MediaCodecVideo::setSpeed(double sp){
    this->speed = sp;
}

AMediaCodec * MediaCodecVideo::createVideoMediaCodec(){
    AMediaFormat *format = AMediaFormat_new();
    if (!videoConfigInfo) {
        videoConfigInfo = VideoConfigInfo::defaultBuild();
    }
    const char * videoMine = videoConfigInfo->videoMime;
    int width = videoConfigInfo->width;
    int height =  videoConfigInfo->height;
    int maxBitRate =  1024 * videoConfigInfo->maxBps;
    int fps = (int)(videoConfigInfo->fps * speed);
    int ivf = videoConfigInfo->ifi;
    int colorFormat = videoConfigInfo->colorFormat;
    delete videoConfigInfo;
    videoConfigInfo = nullptr;
    AMediaFormat_setString(format,AMEDIAFORMAT_KEY_MIME,videoMine);
    AMediaFormat_setInt32(format,AMEDIAFORMAT_KEY_WIDTH,width);
    AMediaFormat_setInt32(format,AMEDIAFORMAT_KEY_HEIGHT,height);
    AMediaFormat_setInt32(format,AMEDIAFORMAT_KEY_BIT_RATE, maxBitRate);
    AMediaFormat_setInt32(format,AMEDIAFORMAT_KEY_FRAME_RATE,fps);
    AMediaFormat_setInt32(format,AMEDIAFORMAT_KEY_I_FRAME_INTERVAL,ivf);
    // yuv 420sp nv21 // 19 - i4205
    AMediaFormat_setInt32(format,AMEDIAFORMAT_KEY_COLOR_FORMAT,colorFormat);
    AMediaCodec *mediaCodec = AMediaCodec_createEncoderByType(videoMine);
    mMediaFormat = format;
    int ret = AMediaCodec_configure(mediaCodec,format, nullptr, nullptr,AMEDIACODEC_CONFIGURE_FLAG_ENCODE);
    if (ret != AMEDIA_OK){
        LOGCATE("video mediacode config error:%d",ret);
        return nullptr;
    }
    return mediaCodec;
}

int64_t MediaCodecVideo::getTotalTime(){
    return totalTime;
}

void MediaCodecVideo::setVideoConfigInfo(VideoConfigInfo* info){
    if (!this->videoConfigInfo) {
        this->videoConfigInfo = new VideoConfigInfo;
        VideoConfigInfo::copy(videoConfigInfo,info);
    }
}

void MediaCodecVideo::destroy() {
    isRunning = false;
    if (videoConfigInfo) {
        delete videoConfigInfo;
        videoConfigInfo = 0;
    }
    encodeThread->join();
    if (mMediaFormat){
        AMediaFormat_delete(mMediaFormat);
    }
    if (mMediaCodec) {
        AMediaCodec_stop(mMediaCodec);
        AMediaCodec_delete(mMediaCodec);
    }
    do {
        NativeOpenGLImage *videoItem = videoQueue.popFirst();
        if (videoItem) {
            NativeOpenGLImageUtil::FreeNativeImage(videoItem);
            delete videoItem;
        }
    } while (videoQueue.size() > 0);

    delete encodeThread;
}

void MediaCodecVideo::putData(NativeOpenGLImage* image) {
    auto overItem = videoQueue.pushLast(image);
    if (overItem) {
        NativeOpenGLImageUtil::FreeNativeImage(overItem);
        delete overItem;
    }
}

void MediaCodecVideo::loopEncode(MediaCodecVideo* codecVideo) {
    while (codecVideo->isRunning){
        // 这里暂时不用rgba来填充数据
        NativeOpenGLImage * image = codecVideo->videoQueue.popFirst();
        if (image == nullptr) {
            av_usleep(20 * 1000);
            continue;
        }
        int widthV = image->width;
        int heightV = image->height;
        int i420Size = widthV * heightV * 3/2;

        uint8_t* localI420Data;
        if (image->format == IMAGE_FORMAT_RGBA) {
            localI420Data = new uint8_t [i420Size];
            yuvRgbaToI420(image->ppPlane[0],localI420Data,widthV,heightV);
        } else {
            localI420Data = image->ppPlane[0];
        }
        int inputIndex = AMediaCodec_dequeueInputBuffer(codecVideo->mMediaCodec,1000);
        if (inputIndex >= 0) {
            size_t inputBufferSize;
            uint8_t *inputData = AMediaCodec_getInputBuffer(codecVideo->mMediaCodec, inputIndex,&inputBufferSize);
            memcpy(inputData,localI420Data,i420Size);
            if (codecVideo -> startNanoTime == 00L) {
                codecVideo->startNanoTime = GetSysNanoTime();
            }
            int64_t timeStamp = (GetSysNanoTime() - codecVideo -> startNanoTime) / 1000;
            int64_t nowTimeStamp = (double)timeStamp / codecVideo -> speed;
            codecVideo -> totalTime = nowTimeStamp;
//            LOGCATE("打印结果视频 当前速度:%lf 打印时间戳微妙：%lld   时间s:%lf",codecVideo->speed,nowTimeStamp,nowTimeStamp/1000.0/1000.0);
            AMediaCodec_queueInputBuffer(codecVideo->mMediaCodec,inputIndex,0,i420Size,nowTimeStamp,0);
        }
        AMediaCodecBufferInfo bufferInfo;
        int outIndex = AMediaCodec_dequeueOutputBuffer(codecVideo->mMediaCodec,&bufferInfo,1000);
//        LOGCATE("log current output index:%d  time:%lld",outIndex,bufferInfo.presentationTimeUs);
        if (outIndex >= 0) {
            size_t outputBufferSize;
            auto outputData = AMediaCodec_getOutputBuffer(codecVideo->mMediaCodec,outIndex,&outputBufferSize);
//            LOGCATE("打印结果视频当前速度：%lf    pts:%lld   时间:%lf",codecVideo->speed,bufferInfo.presentationTimeUs,bufferInfo.presentationTimeUs / 1000.0 / 1000.0);
            codecVideo -> outputDataListener(2,&bufferInfo,outputData);
            AMediaCodec_releaseOutputBuffer(codecVideo->mMediaCodec,outIndex, false);
        } else if (outIndex == AMEDIACODEC_INFO_OUTPUT_FORMAT_CHANGED) {
            codecVideo -> outputFmtChangedListener(2,AMediaCodec_getOutputFormat(codecVideo->mMediaCodec));
        }
        if (image->format == IMAGE_FORMAT_RGBA) delete [] localI420Data;
        NativeOpenGLImageUtil::FreeNativeImage(image);
        delete image;
    }
    LOGCATE("all video mediacode has over");
}
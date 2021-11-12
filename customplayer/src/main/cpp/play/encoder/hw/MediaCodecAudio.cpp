//
// Created by Admin on 2021/7/16.
//

#include <encoder/hw/MediaCodecAudio.h>
#include <media/NdkMediaCodec.h>
#include <utils/utils.h>
#include <utils/CustomGLUtils.h>

extern "C" {
#include <libavutil/time.h>
}

void MediaCodecAudio::setOutputDataListener(OutputFmtChangedListener changedListener,OutputDataListener dataListener){
    this->outputFmtChangedListener = changedListener;
    this->outputDataListener = dataListener;
}

void MediaCodecAudio::startEncode(){
    if (isRunning) return;
    mMediaCodec = createAudioMediaCodec();
    if (mMediaCodec == nullptr) {
        LOGCATE("audio mediacodec create failed");
        return;
    }
    AMediaCodec_start(mMediaCodec);
    encodeThread = new std::thread(loopEncode, this);
    isRunning = true;
}

AMediaCodec * MediaCodecAudio::createAudioMediaCodec(){
    AMediaFormat *format = AMediaFormat_new();
    const char * mine = DEFAULT_AUDIO_MIME;
    int frameSize = DEFAULT_AUDIO_FRAME_SIZE_SINGLE * DEFAULT_AUDIO_CHANNEL_COUNT;
    const char * audioMine = DEFAULT_AUDIO_MIME;
    int sampleRate = DEFAULT_AUDIO_FREQUENCY;
    int channelCount = DEFAULT_AUDIO_CHANNEL_COUNT;
    int maxBitRate = 1024 * DEFAULT_AUDIO_MAX_BPS;
    AMediaFormat_setString(format,AMEDIAFORMAT_KEY_MIME,audioMine);
    AMediaFormat_setInt32(format,AMEDIAFORMAT_KEY_SAMPLE_RATE,sampleRate);
    AMediaFormat_setInt32(format,AMEDIAFORMAT_KEY_CHANNEL_COUNT,channelCount);
    if (mine == audioMine){
        AMediaFormat_setInt32(format,AMEDIAFORMAT_KEY_AAC_PROFILE,2);
    }
    AMediaFormat_setInt32(format,AMEDIAFORMAT_KEY_BIT_RATE,maxBitRate);
    AMediaFormat_setInt32(format,AMEDIAFORMAT_KEY_MAX_INPUT_SIZE,frameSize);
    AMediaCodec *mediaCodec = AMediaCodec_createEncoderByType(mine);
    mMediaFormat = format;
    int ret = AMediaCodec_configure(mediaCodec,format, nullptr, nullptr,AMEDIACODEC_CONFIGURE_FLAG_ENCODE);
    if (ret != AMEDIA_OK){
        LOGCATE("audio mediacode config error:%d",ret);
        return nullptr;
    }
    return mediaCodec;
}

void MediaCodecAudio::destroy() {
    isRunning = false;
    encodeThread->join();
    if (mMediaFormat){
        AMediaFormat_delete(mMediaFormat);
    }
    if (mMediaCodec) {
        AMediaCodec_stop(mMediaCodec);
        AMediaCodec_delete(mMediaCodec);
    }
    do {
        AudioRecordItemInfo *audioItem = audioQueue.popFirst();
        if (audioItem) audioItem->recycle();
    } while (audioQueue.size() > 0);

    delete encodeThread;
}

void MediaCodecAudio::putData(uint8_t* data,int length) {
    auto item = new AudioRecordItemInfo;
    auto itemData = new uint8_t [length];
    memcpy(itemData,data,length);
    item->data = itemData;
    item->nb_samples = length;
    audioQueue.pushLast(item);
}
void MediaCodecAudio::loopEncode(MediaCodecAudio* codecAudio) {
    auto startNano = GetSysNanoTime();
    while (codecAudio->isRunning){
        auto * audioBean = codecAudio->audioQueue.popFirst();
        if (audioBean == nullptr) {
            av_usleep(10 * 1000);
            continue;
        }
        int inputIndex = AMediaCodec_dequeueInputBuffer(codecAudio->mMediaCodec,1000);
        if (inputIndex >= 0) {
            size_t inputBufferSize;
            uint8_t *inputData = AMediaCodec_getInputBuffer(codecAudio->mMediaCodec, inputIndex,&inputBufferSize);
            memcpy(inputData,audioBean->data,audioBean->nb_samples);
//            int64_t timeStamp = (GetSysNanoTime() - startNano) / 1000;
//            LOGCATE("打印音频pts:%lld frameIndex:%d  samples:%d   计算前部分:%lld"
            if (codecAudio->mStartTime == -1LL) {
                codecAudio -> mStartTime = GetSysNanoTime();
            }
            int64_t resultPts = (GetSysNanoTime() - codecAudio->mStartTime) / 1000;
//            LOGCATE("打印每次音频:%lld",resultPts);
            AMediaCodec_queueInputBuffer(codecAudio->mMediaCodec,inputIndex,0,audioBean->nb_samples,resultPts,0);
        }
        AMediaCodecBufferInfo bufferInfo;
        int outIndex = AMediaCodec_dequeueOutputBuffer(codecAudio->mMediaCodec,&bufferInfo,1000);
        if (outIndex >= 0) {
            size_t outputBufferSize;
            auto outputData = AMediaCodec_getOutputBuffer(codecAudio->mMediaCodec,outIndex,&outputBufferSize);
            codecAudio -> outputDataListener(1,&bufferInfo,outputData);
            AMediaCodec_releaseOutputBuffer(codecAudio->mMediaCodec,outIndex, false);
        } else if (outIndex == AMEDIACODEC_INFO_OUTPUT_FORMAT_CHANGED) {
            codecAudio -> outputFmtChangedListener(1,AMediaCodec_getOutputFormat(codecAudio->mMediaCodec));
        }
        audioBean->recycle();
    }
}
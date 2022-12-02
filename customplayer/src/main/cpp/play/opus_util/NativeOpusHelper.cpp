//
// Created by Admin on 2022/11/22.
//
#include <utils/CustomGLUtils.h>
#include <utils/OpenSL_IO.h>
#include "../../play_header/opus_util/NativeOpusHelper.h"

#define OPUS_CHANNELS 1
#define OPUS_FRAME_SIZE OPUS_SAMPLERATE*10/1000*2
#define OPUS_BUFFER_SIZE OPUS_SAMPLERATE * 10 * 16 / (8 * OPUS_CHANNELS) / 1000 * 2
#define OPUS_PATH "/storage/emulated/0/ffmpegtest/opus/test_valid.opus"
// 每帧PCM数据大小：PCM Buffersize=采样率*采样时间*采样位深/8*通道数（Bytes）


NativeOpusHelper* NativeOpusHelper::instance = nullptr;
NativeOpusHelper::NativeOpusHelper() {
    audioRecordPlayHelper = 0;
    instance = this;
    file = tempPcmFile = 0;
    opusEncoder = 0;
    encodeThread = decodeThread = 0;
    startTime = 0L;
    opusPath = nullptr;
    tempPcmPath = nullptr;
}

NativeOpusHelper::~NativeOpusHelper() {
    audioRecordPlayHelper = 0;
    instance = nullptr;
    file = tempPcmFile = 0;
    opusEncoder = 0;
    encodeThread = decodeThread = 0;
    opusPath = nullptr;
    tempPcmPath = nullptr;
}

NativeOpusHelper* NativeOpusHelper::getInstance() {
    return instance;
}

void NativeOpusHelper::realStartCapture(){
    getInstance() ->startTime = GetSysCurrentTime();
    LOGCATE("开始录制");
    getInstance() -> audioRecordPlayHelper->startCapture(NativeOpusHelper::staticReceiveProduceAudio
    ,OPUS_SAMPLERATE,OPUS_CHANNELS,OPUS_FRAME_SIZE,OPUS_BUFFER_SIZE);
//    getInstance() -> audioRecordPlayHelper->startCapture(NativeOpusHelper::staticReceiveProduceAudio);
}

void NativeOpusHelper::staticReceiveProduceAudio(uint8_t* data,int size){
    getInstance()->resolvePcmAudio(data,size);
}


void NativeOpusHelper::init() {
    // 初始化 opus的编码 和opensl的录音
    audioRecordPlayHelper = new AudioRecordPlayHelper;
    opusPath = OPUS_PATH;
    file = fopen(opusPath,"wb+");
    tempPcmPath = getRandomStr("opusfile_", ".pcm", "opus/");
    tempPcmFile = fopen(tempPcmPath,"wb+");
    // 创建opus编码器
    int error;
    opusEncoder = opus_encoder_create(OPUS_SAMPLERATE,OPUS_CHANNELS,OPUS_APPLICATION_RESTRICTED_LOWDELAY,&error);
    int complexity = 3;
    if (opusEncoder) {
        opus_encoder_ctl(opusEncoder,OPUS_SET_VBR(0));//0:CBR, 1:VBR
        opus_encoder_ctl(opusEncoder, OPUS_SET_VBR_CONSTRAINT(true));
        opus_encoder_ctl(opusEncoder, OPUS_SET_BITRATE(AUDIO_BITRATE));
        opus_encoder_ctl(opusEncoder, OPUS_SET_COMPLEXITY(complexity));//8    0~10
        opus_encoder_ctl(opusEncoder, OPUS_SET_SIGNAL(OPUS_SIGNAL_VOICE));
        opus_encoder_ctl(opusEncoder, OPUS_SET_LSB_DEPTH(16));
        opus_encoder_ctl(opusEncoder, OPUS_SET_DTX(0));
        opus_encoder_ctl(opusEncoder, OPUS_SET_INBAND_FEC(0));
        opus_encoder_ctl(opusEncoder, OPUS_SET_PACKET_LOSS_PERC(0));
    }
    LOGCATE("打印opus编码错误:%d 文件:%p 文件地址:%s",error,file,opusPath);
}


void NativeOpusHelper::resolvePcmAudio(uint8_t* utf8Data,int bufferSize){
    LOGCATE("接收到的pcm 样本数量数据:%d",bufferSize);
    int samples = bufferSize / 2; // 单通道，16位采样
    int outPutDataSize = bufferSize / 8;
    uint16_t utf16Data[bufferSize/2];
    uint8_t  outputData[outPutDataSize];
    uint8_t writePcmData[bufferSize];
    LOGCATE("打印opus_buffer_size:%d",OPUS_BUFFER_SIZE);
    memcpy(writePcmData,utf8Data,bufferSize);
    uInt8ToUInt16(utf8Data,utf16Data,bufferSize/2);
    int encodeSize = opus_encode(getInstance() -> opusEncoder,
                                 reinterpret_cast<const opus_int16 *>(utf16Data), samples,
                                 outputData, bufferSize / 8);
    LOGCATE("sapmles:%d encode bytes:%d outputSize:%d OPUS_FRAME_SIZE:%d OPUS_BUFFER_SIZE:%d ",samples
            ,encodeSize,outPutDataSize,OPUS_FRAME_SIZE,OPUS_BUFFER_SIZE);
    if (encodeSize > 0) {
        fwrite(outputData,encodeSize,1,getInstance()->file);
    }
    fwrite(writePcmData,OPUS_BUFFER_SIZE,1,getInstance()->tempPcmFile);
}

void NativeOpusHelper::startEncode() {
    encodeThread = new std::thread(realStartCapture);
}

void NativeOpusHelper::stopEncode() {
    LOGCATE("打印总共录制时长：%lld",(GetSysCurrentTime() - startTime)/1000L/1000L);
    if (audioRecordPlayHelper) audioRecordPlayHelper->stopCapture();
    if (encodeThread) encodeThread->join();
    if (opusEncoder) opus_encoder_destroy(opusEncoder);
    if (file) fclose(file);
    if (tempPcmFile) fclose(tempPcmFile);
//    audioRecordPlayHelper->startPlayBack(OPUS_SAMPLERATE,OPUS_CHANNELS,OPUS_FRAME_SIZE);
}

void NativeOpusHelper::startPlay() {
    decodeThread = new std::thread(realStartDecode);
}

void NativeOpusHelper::playPcm() {
    if (audioRecordPlayHelper) audioRecordPlayHelper->startPlayBack(tempPcmPath,OPUS_SAMPLERATE,OPUS_CHANNELS,
                                                                    OPUS_FRAME_SIZE,OPUS_BUFFER_SIZE);
}

void NativeOpusHelper::realStartDecode() {
    // 直接进行一次性播放
    int error,samples;
    int inPutDataSize = 80;
    int complexity = 8;
    int outputDataSize = OPUS_BUFFER_SIZE;
    uint8_t inputData[inPutDataSize];
    int utf16Size = inPutDataSize*complexity/2;
    uint16_t outputUtf16Data[utf16Size];
    uint8_t convertResultData[inPutDataSize*complexity];


    OpusDecoder *decoder = opus_decoder_create(OPUS_SAMPLERATE, OPUS_CHANNELS, &error);
//    if (decoder) {
//        opus_decoder_ctl(decoder,OPUS_SET_VBR(0));//0:CBR, 1:VBR
//        opus_decoder_ctl(decoder, OPUS_SET_VBR_CONSTRAINT(true));
//        opus_decoder_ctl(decoder, OPUS_SET_BITRATE(AUDIO_BITRATE));
//        opus_decoder_ctl(decoder, OPUS_SET_COMPLEXITY(complexity));//8    0~10
//        opus_decoder_ctl(decoder, OPUS_SET_SIGNAL(OPUS_SIGNAL_VOICE));
//        opus_decoder_ctl(decoder, OPUS_SET_LSB_DEPTH(16));
//        opus_decoder_ctl(decoder, OPUS_SET_DTX(0));
//        opus_decoder_ctl(decoder, OPUS_SET_INBAND_FEC(0));
//        opus_decoder_ctl(decoder, OPUS_SET_PACKET_LOSS_PERC(0));
//    }
    LOGCATE("打印读取地址:%s",OPUS_PATH);
    FILE * readFile = fopen(OPUS_PATH,"rb+");
    if (!decoder || !readFile) {
        LOGCATE("create decoder failed or read file failed,code:%d file:%p",error,readFile);
        return;
    }
    long long int startMillion = GetSysCurrentTime();
    OPENSL_STREAM *stream = OpenSL_IO::android_OpenAudioDevice(OPUS_SAMPLERATE, OPUS_CHANNELS, OPUS_CHANNELS,
                                                               OPUS_BUFFER_SIZE);
    if (!stream) {
        LOGCATE("open audio play stream failed");
        goto InnerEnd;
    }
    while (true){
        LOGCATE("start loop");
//        if (feof(readFile)) {
//            LOGCATE("read file over");
//            break;
//        }
        if (fread(inputData,inPutDataSize,1,readFile) != 1) {
            LOGCATE("read file over1111");
            break;
        }
        error = ferror(readFile);
//        LOGCATE("check readfileerror:%d",error);
        samples =  opus_decode(decoder, inputData, inPutDataSize, reinterpret_cast<opus_int16 *>(outputUtf16Data), OPUS_FRAME_SIZE*2, 0);
        LOGCATE("decode num bytes:%d",samples);
//        if (error < 0) break;
//        fwrite(convertResultData,inPutDataSize*4,1,writeFile);
        if (samples > 0) {
            uInt16ToUInt8(outputUtf16Data,convertResultData,samples);
            OpenSL_IO::android_AudioOut(stream, convertResultData, samples*2);
        }
    }

    InnerEnd:
    OpenSL_IO::android_CloseAudioDevice(stream);
    opus_decoder_destroy(decoder);
    fclose(readFile);
//    fclose(writeFile);
//    getInstance()->audioRecordPlayHelper->startPlayBack(convertPcmPath,OPUS_SAMPLERATE,
//                                                        OPUS_CHANNELS,OPUS_FRAME_SIZE);
    LOGCATE("decoder is over ,总播放时长:%lld",(GetSysCurrentTime() - startMillion)/1000/1000);
}

void NativeOpusHelper::destroy() {
    if (audioRecordPlayHelper) {
        delete audioRecordPlayHelper;
        audioRecordPlayHelper = 0;
    }
    if (decodeThread) delete decodeThread;
}
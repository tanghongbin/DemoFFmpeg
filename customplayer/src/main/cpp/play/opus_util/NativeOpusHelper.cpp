//
// Created by Admin on 2022/11/22.
//
#include <utils/CustomGLUtils.h>
#include <utils/OpenSL_IO.h>
#include "../../play_header/opus_util/NativeOpusHelper.h"

#define OPUS_CHANNELS 1
#define OPUS_FRAME_SIZE OPUS_SAMPLERATE*10/1000
#define OPUS_BUFFER_SIZE 80 * 8
#define OPUS_PATH "/storage/emulated/0/ffmpegtest/opus/test_valid.opus"

NativeOpusHelper* NativeOpusHelper::instance = nullptr;
NativeOpusHelper::NativeOpusHelper() {
    audioRecordPlayHelper = 0;
    instance = this;
    file = 0;
    opusEncoder = 0;
    encodeThread = decodeThread = 0;
    startTime = 0L;
    opusPath = nullptr;
}

NativeOpusHelper::~NativeOpusHelper() {
    audioRecordPlayHelper = 0;
    instance = nullptr;
    file = nullptr;
    opusEncoder = 0;
    encodeThread = decodeThread = 0;
    opusPath = nullptr;
}

NativeOpusHelper* NativeOpusHelper::getInstance() {
    return instance;
}

void NativeOpusHelper::realStartCapture(){
    getInstance() ->startTime = GetSysCurrentTime();
    LOGCATE("开始录制");
    getInstance() -> audioRecordPlayHelper->startCapture(NativeOpusHelper::staticReceiveProduceAudio
    ,OPUS_SAMPLERATE,OPUS_CHANNELS,OPUS_FRAME_SIZE);
//    getInstance() -> audioRecordPlayHelper->startCapture(NativeOpusHelper::staticReceiveProduceAudio);
}

void NativeOpusHelper::staticReceiveProduceAudio(uint8_t* data,int size){
    getInstance()->resolvePcmAudio(data,size);
}

void NativeOpusHelper::resolvePcmAudio(uint8_t* data,int size){
    int outPutDataSize = OPUS_BUFFER_SIZE/8;
    int intputBufferSize = BUFFER_SIZE/2;
    auto* inputData = new uint16_t [intputBufferSize];
    uint8_t  outputData[outPutDataSize];
    uInt8ToUInt16(data,inputData,intputBufferSize);
    int encodeSize = opus_encode(getInstance() -> opusEncoder,
                                 reinterpret_cast<const opus_int16 *>(inputData), OPUS_FRAME_SIZE,
                                 outputData, outPutDataSize);
    LOGCATE("encode error:%d outputSize:%d",encodeSize,outPutDataSize);
    if (encodeSize > 0) {
        fwrite(outputData,outPutDataSize,1,getInstance()->file);
    }
}

void NativeOpusHelper::init() {
    // 初始化 opus的编码 和opensl的录音
    audioRecordPlayHelper = new AudioRecordPlayHelper;
    opusPath = OPUS_PATH;
    file = fopen(opusPath,"wb+");
    // 创建opus编码器
    int error;
    opusEncoder = opus_encoder_create(OPUS_SAMPLERATE,OPUS_CHANNELS,OPUS_APPLICATION_RESTRICTED_LOWDELAY,&error);
    int complexity = 8;
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

void NativeOpusHelper::startEncode() {
    encodeThread = new std::thread(realStartCapture);
}

void NativeOpusHelper::stopEncode() {
    LOGCATE("打印总共录制时长：%lld",(GetSysCurrentTime() - startTime)/1000L/1000L);
    if (audioRecordPlayHelper) audioRecordPlayHelper->stopCapture();
    if (encodeThread) encodeThread->join();
    if (opusEncoder) opus_encoder_destroy(opusEncoder);
    if (file) fclose(file);
//    audioRecordPlayHelper->startPlayBack(OPUS_SAMPLERATE,OPUS_CHANNELS,OPUS_FRAME_SIZE);
}

void NativeOpusHelper::startPlay() {
    decodeThread = new std::thread(realStartDecode);
}

void NativeOpusHelper::realStartDecode() {
    // 直接进行一次性播放
    int error,samples;
    int inPutDataSize = 80;
    auto* inputData = new uint8_t [inPutDataSize];
    uint16_t outputData[inPutDataSize*2];
    uint8_t convertResultData[inPutDataSize*4];

    OpusDecoder *decoder = opus_decoder_create(OPUS_SAMPLERATE, OPUS_CHANNELS, &error);
    LOGCATE("打印读取地址:%s",getInstance()->opusPath);
    FILE * readFile = fopen(getInstance() ->opusPath,"rb+");
    const char *convertPcmPath = getRandomStr("opusfile_", ".pcm", "opus/");
    FILE * writeFile = fopen(convertPcmPath,"wb+");
    if (!decoder || !readFile) {
        LOGCATE("create decoder failed or read file failed,code:%d file:%p",error,readFile);
        return;
    }
    long long int startMillion = GetSysCurrentTime();
    LOGCATE("开始播放");
    OPENSL_STREAM *stream = OpenSL_IO::android_OpenAudioDevice(OPUS_SAMPLERATE, OPUS_CHANNELS, OPUS_CHANNELS,
                                                               OPUS_FRAME_SIZE);
    if (!stream) goto InnerEnd;
    while (true){
        if (feof(readFile)) {
            LOGCATE("read file over");
            break;
        }
        if (fread(inputData,inPutDataSize,1,readFile) != 1) break;
        error = ferror(readFile);
//        LOGCATE("check readfileerror:%d",error);
        error =  opus_decode(decoder, inputData, inPutDataSize, reinterpret_cast<opus_int16 *>(outputData), OPUS_FRAME_SIZE, 0);
        LOGCATE("decode num bytes:%d",error);
//        if (error < 0) break;
        uInt16ToUInt8(outputData,convertResultData,inPutDataSize*2);
//        fwrite(convertResultData,inPutDataSize*4,1,writeFile);
        samples = OpenSL_IO::android_AudioOut(stream, convertResultData, inPutDataSize*4);
        LOGCATE("playback samples:%d",samples);
        if (samples < 0) {
//            LOGCATE("android_AudioOut failed !\n");
        }
    }

    InnerEnd:
    OpenSL_IO::android_CloseAudioDevice(stream);
    opus_decoder_destroy(decoder);
    fclose(readFile);
    fclose(writeFile);
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
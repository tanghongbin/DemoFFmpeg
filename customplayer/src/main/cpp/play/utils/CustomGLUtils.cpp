//
// Created by Admin on 2020/8/24.
//




#include "GLES3/gl3.h"
#include "malloc.h"
#include "cstdlib"
#include <android/native_window_jni.h>
#include <android/native_window.h>
#include <pthread.h>
#include <thread>
#include <iostream>
#include <utils/utils.h>
#include <utils/CustomGLUtils.h>
#include <libyuv/rotate.h>
#include <libyuv/convert.h>
#include <libyuv/scale.h>
#include <chrono>


extern "C" {
#include <libavformat/avformat.h>
#include <libavutil/imgutils.h>
#include <libavutil/opt.h>
#include <lame/lame.h>
}


void encode(AVCodecContext *pContext, AVFrame *pFrame, AVPacket *pPacket, FILE *pFile);

GLuint LoadShader(GLenum shaderType, const char *pSource) {
    GLuint shader = 0;
    shader = glCreateShader(shaderType);
    if (shader == 0) {
        LOGCATE("create shader failed");
        return shader;
    };
//        LOGCATE("createshader success");
    glShaderSource(shader, 1, &pSource, nullptr);
    glCompileShader(shader);
    GLint compiled = 0;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &compiled);
    if (!compiled) {
        GLint infoLen = 0;
        glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &infoLen);
        if (infoLen) {
            char *buf = (char *) malloc(sizeof(infoLen));
            if (buf) {
                glGetShaderInfoLog(shader, infoLen, NULL, buf);
                LOGCATE("GLUtils::LoadShader Could not compile shader %d:\n%s\n", shaderType,
                        buf);
                free(buf);
            }
            glDeleteShader(shader);
            shader = 0;
        }
    }
    return shader;
}


unsigned int TextureFromFile(const std::string path, std::string &directory)
{
    std::string filename = std::string(path);
    filename = directory + '/' + filename;
    LOGCATE("log fileName:%s",filename.c_str());
    unsigned int textureID;
    glGenTextures(1, &textureID);

    int width, height, nrComponents;
    unsigned char *data = stbi_load(filename.c_str(), &width, &height, &nrComponents, 0);
    if (data)
    {
        GLenum format;
        if (nrComponents == 1)
            format = GL_RED;
        else if (nrComponents == 3)
            format = GL_RGB;
        else if (nrComponents == 4)
            format = GL_RGBA;

        glBindTexture(GL_TEXTURE_2D, textureID);
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        stbi_image_free(data);
    }
    else
    {
        LOGCATE("Texture failed to load at path: %s",path.c_str());
        stbi_image_free(data);
    }
    LOGCATE("log textureId:%d path:%s directory:%s",textureID,path.c_str(),directory.c_str());

    return textureID;
}


GLuint CreateProgram(const char *pVertexShaderSource, const char *pFragShaderSource) {
    GLuint vertexShaderHandle, fragShaderHandle;
    return CreateProgram(pVertexShaderSource, pFragShaderSource, vertexShaderHandle,
                         fragShaderHandle);
}

long long GetSysCurrentTime() {
    struct timeval time;
    gettimeofday(&time, NULL);
    long long curTime = ((long long) (time.tv_sec)) * 1000 + time.tv_usec / 1000;
    return curTime;
}

long long GetSysNanoTime() {
    timespec now;
    clock_gettime(CLOCK_MONOTONIC, &now);
    return now.tv_sec * 1000000000LL + now.tv_nsec;
}

/**
 * 设置渲染窗口的宽高
 * case 1 - 视频 宽>高（横屏视频），
 *      1.1 手机设备 高>宽(竖屏) , 渲染的宽要 = 设备的宽，高等比缩小
 *      1.2 手机设备 宽>高（横屏）, 渲染的宽 = 设备的宽，要比较设备与视频宽的大小，进行缩小或放大
 *
 * case 2 - 视频 高>宽 (竖屏视频)
 *      2.1 手机设备 高>宽(竖屏) , 渲染的宽要 = 设备的宽，要比较设备与视频宽的大小，进行缩小或放大
 *      2.2 手机设备 宽>高（横屏）, 渲染的高 = 设备的高，宽进行等比缩小
 *
 * @param nativeWindowWidth
 * @param nativeWindowHeight
 * @param videoWidth
 * @param videoHeight
 * @param renderWidth
 * @param renderHeight
 */
void setupRenderDimension(int orientation,int nativeWindowWidth, int nativeWindowHeight, int videoWidth,
        int videoHeight,int *renderWidth, int *renderHeight) {

    // 横屏视频
    if (videoWidth > videoHeight) {
        if (nativeWindowHeight > nativeWindowWidth) {
            float scaleSmallWidthRation = videoWidth * 1.0f / nativeWindowWidth;
            *renderWidth = nativeWindowWidth;
            *renderHeight = static_cast<int>(videoHeight / scaleSmallWidthRation);
        } else {
            float scaleVideoRation = videoWidth * 1.0f / videoHeight;
            *renderWidth = nativeWindowWidth;
            *renderHeight = static_cast<int>(nativeWindowWidth / scaleVideoRation);
            if (*renderHeight > nativeWindowHeight) {
                *renderHeight = nativeWindowHeight;
                *renderWidth = static_cast<int>(nativeWindowHeight * scaleVideoRation);
            }
        }
    } else {
        // 竖屏视频
        float scaleSmallHeightRation = videoHeight * 1.0f / nativeWindowHeight;
        if (nativeWindowHeight > nativeWindowWidth) {
            *renderWidth = nativeWindowWidth;
            *renderHeight = nativeWindowHeight;
        } else {
            *renderHeight = nativeWindowHeight;
            *renderWidth = static_cast<int>(videoWidth / scaleSmallHeightRation);
        }
    }
    LOGCATE("nativeWindowWidth:%d nativeWindowHeight:%d\n "
            "videoWidth:%d videoHeight:%d \n"
            "renderWidth:%d renderHeight:%d",
            nativeWindowWidth,nativeWindowHeight,
            videoWidth,videoHeight,
            *renderWidth,*renderHeight);
}

void syslog_print(void *ptr, int level, const char *fmt, va_list vl) {
    switch (level) {
        case AV_LOG_DEBUG:
            LOGCATE_LEVEL(ANDROID_LOG_VERBOSE, fmt, vl);
            break;
        case AV_LOG_VERBOSE:
            LOGCATE_LEVEL(ANDROID_LOG_DEBUG, fmt, vl);
            break;
        case AV_LOG_INFO:
            LOGCATE_LEVEL(ANDROID_LOG_INFO, fmt, vl);
            break;
        case AV_LOG_WARNING:
            LOGCATE_LEVEL(ANDROID_LOG_WARN, fmt, vl);
            break;
        case AV_LOG_ERROR:
            LOGCATE_LEVEL(ANDROID_LOG_ERROR, fmt, vl);
            break;
    }
}

void sys_log_init() {
    av_log_set_callback(syslog_print);
    av_log_set_level(AV_LOG_DEBUG);
    LOGCATE("sys_log_init has successed");
}

uint8_t *fillArrayToFrame(AVPixelFormat avPixelFormat, AVFrame *frame) {
    int in_size = av_image_get_buffer_size(avPixelFormat, frame->width, frame->height, 1);
    uint8_t *in_buffer = static_cast<uint8_t *>(av_malloc(in_size));
    av_image_fill_arrays(frame->data, frame->linesize, in_buffer, AV_PIX_FMT_YUV420P, frame->width,
                         frame->height, 1);
    return in_buffer;
}

/**
 * nv21 转换成yuv420p
 * @param image_src
 * @param image_dst
 * @param image_width
 * @param image_height
 */
void NvToYUV420p(const uint8_t *image_src, uint8_t *image_dst, int image_width, int image_height) {
    uint8_t *p = image_dst;
    memcpy(p, image_src, image_width * image_height * 3 / 2);
    const uint8_t *pNV = image_src + image_width * image_height;
    uint8_t *pU = p + image_width * image_height;
    uint8_t *pV = p + image_width * image_height + ((image_width * image_height) >> 2);
    for (int i = 0; i < (image_width * image_height) / 2; i++) {
        if ((i % 2) == 0)
            *pV++ = *(pNV + i);
        else
            *pU++ = *(pNV + i);
    }
}

void checkNegativeError(int ret,const char* error){
    if (ret < 0) {
        LOGCATE("retCode:%d errorStr:%s",ret,error);
    }
}

bool checkNegativeReturn(int ret, const char *string) {
    if (ret < 0) {
        LOGCATE("------------:retCode:%d  msg:%s  ffmpegErrorStr:%s", ret, string, av_err2str(ret));
        return true;
    }
    return false;
}

GLuint CreateProgram(const char *pVertexShaderSource, const char *pFragShaderSource,
                     GLuint &vertexShaderHandle, GLuint &fragShaderHandle) {
    GLuint geometry;
    return CreateProgram(pVertexShaderSource,pFragShaderSource,vertexShaderHandle,fragShaderHandle,
                         nullptr,geometry);
}

void glCheckError(const char *pGLOperation){
    GLenum error = glGetError();
    LOGCATE("GLUtils::glCheckError GL Operation %s() glError (0x%x)\n", pGLOperation, error);
}

void yuvNv21To420p(uint8_t *nv21Data,uint8_t * i420RorateDst, int width,int height, libyuv::RotationMode mode) {
    int frameSize = width * height * 3 / 2;
    uint8_t * i420DstData = new uint8_t [frameSize];
    memset(i420DstData,0x00,frameSize);
    memset(i420RorateDst,0x00,frameSize);
    uint8_t * nv21YData = nv21Data;
    uint8_t * nv21UVData = nv21Data + width * height;
    uint8_t * i420UData = i420DstData + width * height;
    uint8_t * i420VData = i420DstData + width * height * 5 / 4;//        LOGCATE("srcData:%p dstData:%p",nv21Data,i420DstData);
    libyuv::NV21ToI420((const uint8_t *)nv21YData,width,
                       (const uint8_t *)nv21UVData,width,
                       (uint8_t *) i420DstData,width,
                       (uint8_t *) i420UData,width >> 1,
                       (uint8_t *) i420VData,width >> 1,width,height);
    libyuv::I420Rotate(i420DstData, width,
                       i420UData, width >> 1,
                       i420VData, width >> 1,
                       i420RorateDst, height,
                       i420RorateDst + width * height, height >> 1,
                       i420RorateDst + width* height* 5/4, height >> 1,
                       width, height, mode);
    delete [] i420DstData;
}


#pragma clang diagnostic push
#pragma ide diagnostic ignored "ArgumentSelectionDefects"
void yuvI420Rotate(uint8_t *i420Src, uint8_t * i420RorateDst, int width, int height, libyuv::RotationMode mode,bool isMirror){
    uint8_t * uData = i420Src + width * height;
    uint8_t * vData = i420Src + width * height * 5 / 4;
    uint8_t * tmpDst = i420RorateDst;
    if (isMirror) tmpDst = new uint8_t [width * height *3/2];
    libyuv::I420Rotate(i420Src, width,
                       uData, width >> 1,
                       vData, width >> 1,
                       tmpDst, height,
                       tmpDst + width * height, height >> 1,
                       tmpDst + width* height* 5/4, height >> 1,
                       width, height, mode);
    if (isMirror){
        int localW = height;
        int localH = width;
        libyuv::I420Mirror(tmpDst, height,
                           tmpDst + width * height, height >> 1,
                           tmpDst + width * height * 5 / 4, height >> 1,
                           i420RorateDst, height,
                           i420RorateDst + width * height, height >> 1,
                           i420RorateDst + width* height* 5/4, height >> 1,
                           localW, localH);
        delete [] tmpDst;
    }
}
#pragma clang diagnostic pop

void yuvI420RotateVertical(uint8_t *i420Src, uint8_t * i420RorateDst, int width, int height){
    uint8_t * uData = i420Src + width * height;
    uint8_t * vData = i420Src + width * height * 5 / 4;
    libyuv::I420Rotate(i420Src, width,
                       uData, width >> 1,
                       vData, width >> 1,
                       i420RorateDst, height,
                       i420RorateDst + width * height, width >> 1,
                       i420RorateDst + width* height* 5/4, width >> 1,
                       width, height, libyuv::kRotate180);
}

void yuvRgbaToI420(uint8_t *rgbaData, uint8_t * i420Dst, int width, int height){
    uint8_t * i420U = i420Dst + width * height;
    uint8_t * i420V = i420Dst + width * height * 5 / 4;
    libyuv::ABGRToI420(rgbaData,width * 4,
                       i420Dst,width,
                       i420U,width / 2,
                       i420V,width / 2,width,height);
}

void yuvRgbaToNv21(uint8_t *rgbaData, uint8_t * nv21Dst, int width, int height){
    uint8_t* i420Tmp = new uint8_t [width * height * 3/2];
    uint8_t * i420U = i420Tmp + width * height;
    uint8_t * i420V = i420Tmp + width * height * 5 / 4;
    libyuv::RGBAToI420(rgbaData,width * 4,
                       i420Tmp,width,
                       i420U,width / 2,
                       i420V,width / 2,width,height);
    libyuv::I420ToNV21(i420Tmp,width,
            i420Tmp + width * height,width / 2,
            i420Tmp + width * height * 5/4,width / 2,
            nv21Dst,width,
            nv21Dst + width * height,width,
            width,height);
    delete [] i420Tmp;
}



void yuvI420Scale(uint8_t *i420Src, uint8_t * i420Dst, int srcWidth, int srcHeight,int dstWidth,int dstHeight){
    uint8_t *srcUData = i420Src + srcWidth * srcHeight;
    uint8_t *srcVData = i420Src + srcWidth * srcHeight * 5/4;
    uint8_t *dstUData = i420Dst + dstWidth * dstHeight;
    uint8_t *dstVData = i420Dst + dstWidth * dstHeight * 5/4;
    libyuv::I420Scale(i420Src,srcWidth,
            srcUData,srcWidth >> 1,
            srcVData,srcWidth >> 1,
            srcWidth,srcHeight,
            i420Dst,dstWidth,
            dstUData,dstWidth >> 1,
            dstVData,dstWidth >> 1,dstWidth,dstHeight,libyuv::kFilterNone);
}

void yuvNv12ToI420(uint8_t* nv12src,uint8_t* i420Dst,int width,int height){
    libyuv::NV12ToI420(nv12src,width,
            nv12src + width * height,width,
                       i420Dst,width,
                       i420Dst + width * height,width >> 1,
                       i420Dst + width * height * 5 / 4,width >> 1,width,height);
}

void createFolderIfNotExist(char * _f){
    bool isFolder = true;
    if (strstr(_f,".")){
        isFolder = false;
    }
    CustomSafeQueue<char *> safeQueue;
    std::string lastStr;
    char filePath [128];
    strcpy(filePath,_f);
    char * splitResult = strtok(filePath,"/");
    while (splitResult != NULL){
        safeQueue.pushLast(splitResult);
        splitResult = strtok(NULL,"/");
    }
    lastStr = safeQueue.getLast();
    std::string resultPath = "";
    while (safeQueue.size() > 0) {
        std::string item = safeQueue.popFirst();
        if (!isFolder && lastStr == item) continue;
        resultPath = resultPath + "/" + item;
        if (access(resultPath.c_str(),0) < 0){
            mkdir(resultPath.c_str(),0777);
            LOGCATE("文件不存在:%s",resultPath.c_str());
        }
    }
    LOGCATE("创建成功，最终地址为:%s",resultPath.c_str());
}

GLuint CreateProgram(const char *pVertexShaderSource, const char *pFragShaderSource,
                     GLuint &vertexShaderHandle, GLuint &fragShaderHandle,const char *geometryShaderSouce,
                     GLuint &geometryHandle){
    GLuint program = 0;
    vertexShaderHandle = LoadShader(GL_VERTEX_SHADER, pVertexShaderSource);
    if (!vertexShaderHandle){
        LOGCATE("GLUtils::CreateProgram vertexShaderHandle error:");
        return program;
    }
    fragShaderHandle = LoadShader(GL_FRAGMENT_SHADER, pFragShaderSource);
    if (!fragShaderHandle) {
        LOGCATE("GLUtils::CreateProgram fragShaderHandle error:");
        return program;
    }

    program = glCreateProgram();
    if (program) {
        glAttachShader(program, vertexShaderHandle);
        glCheckError("glAttachShader vertexShaderHandle");
        glAttachShader(program, fragShaderHandle);
        glCheckError("glAttachShader fragShaderHandle");
        glLinkProgram(program);
        GLint linkStatus = GL_FALSE;
        glGetProgramiv(program, GL_LINK_STATUS, &linkStatus);

        glDetachShader(program, vertexShaderHandle);
        glDeleteShader(vertexShaderHandle);
        vertexShaderHandle = 0;
        glDetachShader(program, fragShaderHandle);
        glDeleteShader(fragShaderHandle);
        fragShaderHandle = 0;
        if (linkStatus != GL_TRUE) {
            GLint bufLength = 0;
            glGetProgramiv(program, GL_INFO_LOG_LENGTH, &bufLength);
            if (bufLength) {
                char *buf = (char *) malloc((size_t) bufLength);
                if (buf) {
                    glGetProgramInfoLog(program, bufLength, NULL, buf);
                    LOGCATE("GLUtils::CreateProgram Could not link program:\n%s\n", buf);
                    free(buf);
                }
            }
            glDeleteProgram(program);
            program = 0;
        }
    }
    LOGCATE("GLUtils::CreateProgram program = %d", program);
    return program;
}

void DeleteProgram(GLuint program) {
    if (program) {
        glUseProgram(0);
        glDeleteProgram(program);
        program = 0;
    }
}

size_t getFileSize(const char * filePath){
    FILE* fp = fopen(filePath, "r+");
    fseek(fp, 0, SEEK_END);
    size_t size = ftell(fp);
    fclose(fp);
    return size; //单位是：byte
}

bool startWith(const char * allStr,const char * prefix){
    std::string resultAll = allStr;
    std::string resultPrefix = prefix;
    bool boolResult = resultAll.size() >= resultPrefix.size() &&
                      resultAll.compare(0, resultPrefix.size(), prefix) == 0;
    return boolResult;
}

bool endWith(const char * allStr,const char * prefix){
    return 1;
}

std::thread *thread;

void testParams(int age, const char *name, const char *title) {
    int count = 0;
    while (true) {
        count++;
        LOGCATE("打印书的年龄:%d 名字:%s 标题:%s 当前次数:%d", age, name, title, count);
        if (count > 10) {
            break;
        }
        sleep(1);
    }
//    LOGCATE("print thread id:%d",thread->get_id());
//    delete thread;
//    LOGCATE("after print thread id:%d",thread->get_id());
    LOGCATE("testParams has finished");
}


void testLocalThread() {
    thread = new std::thread(testParams, 1, "23232", "adfasdfsfs");
}

const char *getCharStrFromJstring(JNIEnv *jniEnv, jstring str) {
    const char *result = jniEnv->GetStringUTFChars(str, 0);
    return result;
}

const char *getRandomStr(const char *prefix, const char *suffix) {
    return getRandomStr(prefix, suffix, "");
}


const char *getRandomStr(const char *prefix, const char *suffix, const char *subDir) {
    long long start = GetSysCurrentTime();
    std::string dirs = "/storage/emulated/0/ffmpegtest/";
    std::string result = dirs + subDir + prefix + std::to_string(start) + suffix;
    char *resultChar = new char[100];
    strcpy(resultChar, result.c_str());
    return resultChar;
}

const int getRandomInt(int range){
    return random() % range;
}




jstring getJstringFromCharStr(JNIEnv *jniEnv, const char *str) {
    jstring result = jniEnv->NewStringUTF(str);
    return result;
}

const char *encdoeMp4FromOfficalDemo() {
    const char *filename;
    const AVCodec *codec;
    AVCodecContext *c = NULL;
    int i, ret, x, y;
    FILE *f;
    AVFrame *frame;
    AVPacket *pkt;
    uint8_t endcode[] = {0, 0, 1, 0xb7};

    filename = "/storage/emulated/0/test_out_file.mp4";

    /* find the mpeg1video encoder */
    codec = avcodec_find_encoder(AV_CODEC_ID_H264);
    if (!codec) {
        LOGCATE("can't find encoder");
//        LOGCATE(stderr, "Codec '%s' not found\n", codec_name);
        exit(1);
    }

    c = avcodec_alloc_context3(codec);
    if (!c) {
        LOGCATE("Could not allocate video codec context\n");
        exit(1);
    }

    pkt = av_packet_alloc();
    if (!pkt)
        exit(1);

    /* put sample parameters */
    c->bit_rate = 400000;
    /* resolution must be a multiple of two */
    c->width = 352;
    c->height = 288;
    /* frames per second */
    c->time_base = (AVRational) {1, 25};
    c->framerate = (AVRational) {25, 1};

    /* emit one intra frame every ten frames
     * check frame pict_type before passing frame
     * to encoder, if frame->pict_type is AV_PICTURE_TYPE_I
     * then gop_size is ignored and the output of encoder
     * will always be I frame irrespective to gop_size
     */
    c->gop_size = 10;
    c->max_b_frames = 1;
    c->pix_fmt = AV_PIX_FMT_YUV420P;

    if (codec->id == AV_CODEC_ID_H264)
        av_opt_set(c->priv_data, "preset", "slow", 0);

    /* open it */
    ret = avcodec_open2(c, codec, NULL);
    if (ret < 0) {
        LOGCATE("Could not open codec: %s\n", av_err2str(ret));
        exit(1);
    }

    f = fopen(filename, "w+b");
    if (!f) {
        LOGCATE("Could not open %s\n", filename);
        exit(1);
    }

    frame = av_frame_alloc();
    if (!frame) {
        LOGCATE("Could not allocate video frame\n");
        exit(1);
    }
    frame->format = c->pix_fmt;
    frame->width = c->width;
    frame->height = c->height;

    ret = av_frame_get_buffer(frame, 0);
    if (ret < 0) {
        LOGCATE("Could not allocate the video frame data\n");
        exit(1);
    }

    /* encode 1 second of video */
    for (i = 0; i < 25; i++) {
        fflush(stdout);

        /* make sure the frame data is writable */
        ret = av_frame_make_writable(frame);
        if (ret < 0)
            exit(1);

        /* prepare a dummy image */
        /* Y */
        for (y = 0; y < c->height; y++) {
            for (x = 0; x < c->width; x++) {
                frame->data[0][y * frame->linesize[0] + x] = x + y + i * 3;
            }
        }

        /* Cb and Cr */
        for (y = 0; y < c->height / 2; y++) {
            for (x = 0; x < c->width / 2; x++) {
                frame->data[1][y * frame->linesize[1] + x] = 128 + y + i * 2;
                frame->data[2][y * frame->linesize[2] + x] = 64 + x + i * 5;
            }
        }

        frame->pts = i;

        /* encode the image */
        encode(c, frame, pkt, f);
    }

    /* flush the encoder */
    encode(c, NULL, pkt, f);

    /* add sequence end code to have a real MPEG file */
    if (codec->id == AV_CODEC_ID_MPEG1VIDEO || codec->id == AV_CODEC_ID_MPEG2VIDEO)
        fwrite(endcode, 1, sizeof(endcode), f);
    fclose(f);

    avcodec_free_context(&c);
    av_frame_free(&frame);
    av_packet_free(&pkt);

    return 0;
}

const char *encodeYuvToImageUtils2(const char *filePath) {
    const char *encodeName = "mjpeg";
    const char *ERROR_RESULT = "";
    const char *outfile = "/storage/emulated/0/test_out_file.jpg";
    AVCodec *codec;
    AVCodecContext *codeCtx;
    int i, ret, x, y;
    FILE *inputFile;
    FILE *outputFile;
    AVFrame *frame;
    AVPacket *packet;
    uint8_t endcode[] = {0, 0, 1, 0xb7};

    // 1.初始化编码环境
    codec = avcodec_find_encoder_by_name(encodeName);
    if (!codec) {
        LOGCATE("can't find encoder:%s", encodeName);
        return ERROR_RESULT;
    }

    codeCtx = avcodec_alloc_context3(codec);
    if (!codeCtx) {
        LOGCATE("can't alloc context");
        return ERROR_RESULT;
    }

    packet = av_packet_alloc();
    if (!packet) {
        LOGCATE("can't alloc packet");
        return ERROR_RESULT;
    }

    codeCtx->bit_rate = 400000;

    codeCtx->width = 840;
    codeCtx->height = 1074;

    codeCtx->time_base = AVRational{1, 25};
    codeCtx->framerate = AVRational{25, 1};

    codeCtx->max_b_frames = 1;
    codeCtx->gop_size = 1;
    codeCtx->pix_fmt = AV_PIX_FMT_YUV420P;

    //2.打开编码器
    ret = avcodec_open2(codeCtx, codec, NULL);
    if (ret < 0) {
        LOGCATE("can't open encoder");
        return ERROR_RESULT;
    }

    outputFile = fopen(outfile, "wb");
    inputFile = fopen(filePath, "rb");
    if (!outputFile) {
        LOGCATE("can't open outputfile");
        return ERROR_RESULT;
    }

    if (!inputFile) {
        LOGCATE("can't open inputfile");
        return ERROR_RESULT;
    }

    frame = av_frame_alloc();
    if (!frame) {
        LOGCATE("can't allocate frame");
        return ERROR_RESULT;
    }
    frame->width = codeCtx->width;
    frame->height = codeCtx->height;
    frame->format = codeCtx->pix_fmt;

    /* make sure the frame data is writable */
    ret = av_frame_make_writable(frame);
    if (ret < 0) {
        LOGCATE("frame is not writeable");
        return ERROR_RESULT;
    }

    // 读取yuv数据
    int imageSize = av_image_get_buffer_size(codeCtx->pix_fmt, codeCtx->width, codeCtx->height, 1);
    uint8_t *pic_buffer = static_cast<uint8_t *>(av_malloc(imageSize));
    if (!pic_buffer) {
        LOGCATE("can't allocate pic_buffer");
        return ERROR_RESULT;
    }
    int y_size = codeCtx->width * codeCtx->height;
    avpicture_fill(reinterpret_cast<AVPicture *>(frame), pic_buffer, codeCtx->pix_fmt,
                   codeCtx->width, codeCtx->height);
    if (fread(pic_buffer, 1, y_size * 3 / 2, inputFile) < 0) {
        LOGCATE("can't open intput file");
        return ERROR_RESULT;
    }
    frame->data[0] = pic_buffer;
    frame->data[0] = pic_buffer + y_size;
    frame->data[0] = pic_buffer + y_size * 5 / 4;


    // 3.真正开始编码

    encode(codeCtx, frame, packet, outputFile);

    av_free_packet(packet);

    fclose(inputFile);
    fclose(outputFile);

    LOGCATE("encode successful");
    avcodec_close(codeCtx);
    av_frame_free(&frame);
    av_packet_free(&packet);
    av_free(pic_buffer);


}

void encode(AVCodecContext *enc_ctx, AVFrame *frame, AVPacket *pkt, FILE *outfile) {

    int ret;

    /* send the frame to the encoder */
//    if (frame)
//        LOGCATE("Send frame %3"PRId64"\n", frame->pts);

    ret = avcodec_send_frame(enc_ctx, frame);
    if (ret < 0) {
        LOGCATE("Error sending a frame for encoding\n");
        exit(1);
    }

    while (ret >= 0) {
        ret = avcodec_receive_packet(enc_ctx, pkt);
        if (ret == AVERROR(EAGAIN) || ret == AVERROR_EOF)
            return;
        else if (ret < 0) {
            LOGCATE("Error during encoding\n");
            exit(1);
        }

//        LOGCATE("Write packet %3"PRId64" (size=%5d)\n", pkt->pts, pkt->size);
        fwrite(pkt->data, 1, pkt->size, outfile);
        av_packet_unref(pkt);
    }
}

const char *encodeYuvToImageUtils(const char *filePath) {

    const char *IN_VALID_RESULT = "";
    LOGCATE("打印yuv图片地址:%s", filePath);

    AVFormatContext *pFormatContext;

    uint8_t *picture_buf;
    AVFrame *picture;
    AVPacket *pkt;
    int y_size;
    int got_picture = 0;
    int size;
    int ret = 0;

    FILE *in_file = NULL;
    int width = 840, height = 1074;
    const char *outfile = "/storage/emulated/0/test_out_file.jpg";
    in_file = fopen(filePath, "r+b");

    LOGCATE("open file success");
    pFormatContext = avformat_alloc_context();
    pFormatContext->oformat = av_guess_format("mjpeg", NULL, NULL);
    if (avio_open(&pFormatContext->pb, outfile, AVIO_FLAG_READ_WRITE) < 0) {
        LOGCATE("can't open file");
        return IN_VALID_RESULT;
    }
    AVStream *video_st = avformat_new_stream(pFormatContext, 0);
    if (video_st == NULL) {
        LOGCATE("create new stream failed ");
        return IN_VALID_RESULT;
    }
    //5. 根据流找到解码器id,因为流里面有对应的编码信息
    AVCodecParameters *code_params = video_st->codecpar;
    AVCodec *encoder = avcodec_find_decoder(AV_CODEC_ID_MJPEG);
    if (encoder == nullptr) {
        LOGCATE("can't find decoder");
        return IN_VALID_RESULT;
    }

    //6.创建解码器上下文
    AVCodecContext *pCodecCtx = avcodec_alloc_context3(encoder);
    if (avcodec_parameters_to_context(pCodecCtx, code_params) != 0) {
        LOGCATE("avcodec_parameters_to_context failed");
        return IN_VALID_RESULT;
    }
    pCodecCtx->codec_id = AV_CODEC_ID_MJPEG;
    pCodecCtx->codec_type = AVMEDIA_TYPE_VIDEO;
    pCodecCtx->pix_fmt = AV_PIX_FMT_YUV420P;

    pCodecCtx->width = width;
    pCodecCtx->height = height;

    pCodecCtx->time_base.num = 1;
    pCodecCtx->time_base.den = 25;

    // 7.打开解码器
    int openResult = avcodec_open2(pCodecCtx, encoder, NULL);
    if (openResult < 0) {
        LOGCATE("open codec failed : %d", openResult);
        return IN_VALID_RESULT;
    }

    picture = av_frame_alloc();
    size = av_image_get_buffer_size(pCodecCtx->pix_fmt, pCodecCtx->width, pCodecCtx->height, 1);
    picture_buf = (uint8_t *) av_malloc(size);
    if (!picture_buf) {
        LOGCATE("picture_buf malloc failed");
        return IN_VALID_RESULT;
    }

    avpicture_fill(reinterpret_cast<AVPicture *>(picture), picture_buf, pCodecCtx->pix_fmt,
                   pCodecCtx->width, pCodecCtx->height);


    avformat_write_header(pFormatContext, NULL);
    y_size = pCodecCtx->width * pCodecCtx->height;
    av_new_packet(pkt, y_size * 3);
    //read yuv
    if (fread(picture_buf, 1, y_size * 3 / 2, in_file) < 0) {
        LOGCATE("can't open input file");
        return IN_VALID_RESULT;
    }
    picture->data[0] = picture_buf;
    picture->data[0] = picture_buf + y_size;
    picture->data[0] = picture_buf + y_size * 5 / 4;

    // encode
    ret = avcodec_encode_video2(pCodecCtx, pkt, picture, &got_picture);
    if (ret < 0) {
        LOGCATE("encode error");
        return IN_VALID_RESULT;
    }
    if (got_picture == 1) {
        pkt->stream_index = video_st->index;
        ret = av_write_frame(pFormatContext, pkt);
    }

    av_free_packet(pkt);
    av_write_trailer(pFormatContext);

    LOGCATE("encode successful");
    if (video_st) {
        avcodec_close(pCodecCtx);
        av_free((picture));
        av_free(picture_buf);
    }
    avio_close(pFormatContext->pb);
    avformat_free_context(pFormatContext);
    fclose(in_file);

    return outfile;
}


void *test(void *params) {
    TestParams *testParams = static_cast<TestParams *>(params);
    LOGCATE("in test 书的大小:%d 名字:%s  标题:%s", testParams->age, testParams->bookName,
            testParams->bookTitle);
    pthread_exit(0);
}


//void createThreadForPlay(JNIEnv *pEnv, _jstring *pJstring, _jobject *pJobject) {
//    pthread_t pthread;
//    LOGCATE("prepare open new thread");
//    ThreadParams* params = new ThreadParams;
//    params->pJstring = pJstring;
//    params->pJobject = pJobject;
//    pthread_create(&pthread, NULL, &playNativeMp4, params);
//    LOGCATE("prepare open finished");
//}




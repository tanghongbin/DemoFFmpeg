//
// Created by Admin on 2020/8/24.
//




#include <MyGLRenderContext.h>
#include "GLES3/gl3.h"
#include "malloc.h"
#include "cstdlib"
#include "utils.h"
#include <android/native_window_jni.h>
#include <android/native_window.h>
#include <pthread.h>
#include <thread>


extern "C" {
#include <libavformat/avformat.h>
#include <libavutil/imgutils.h>
}

JavaVM *GLUtilC::mVm = nullptr;

int64_t GLUtilC::master_audio_clock = 0;

void CheckGLError(const char *pGLOperation);

GLuint LoadShader(GLenum shaderType, const char *pSource) {
    GLuint shader = 0;
    shader = glCreateShader(shaderType);
    if (shader) {
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
    }
    return shader;
}



GLuint CreateProgram(const char *pVertexShaderSource, const char *pFragShaderSource) {
    GLuint vertexShaderHandle, fragShaderHandle;
    return CreateProgram(pVertexShaderSource, pFragShaderSource, vertexShaderHandle, fragShaderHandle);
}

long long GetSysCurrentTime()
{
    struct timeval time;
    gettimeofday(&time, NULL);
    long long curTime = ((long long)(time.tv_sec))*1000+time.tv_usec/1000;
    return curTime;
}

GLuint CreateProgram(const char *pVertexShaderSource, const char *pFragShaderSource,
                     GLuint &vertexShaderHandle, GLuint &fragShaderHandle) {
    GLuint program = 0;
    vertexShaderHandle = LoadShader(GL_VERTEX_SHADER, pVertexShaderSource);
//    LOGCATE("GLUtils::CreateProgram vertexShaderHandle = %d", vertexShaderHandle);
    if (!vertexShaderHandle) return program;

    fragShaderHandle = LoadShader(GL_FRAGMENT_SHADER, pFragShaderSource);
    LOGCATE("GLUtils::CreateProgram fragShaderHandle = %d", fragShaderHandle);
    if (!fragShaderHandle) return program;

    program = glCreateProgram();
    if (program) {
        glAttachShader(program, vertexShaderHandle);
        CheckGLError("glAttachShader");
        glAttachShader(program, fragShaderHandle);
        CheckGLError("glAttachShader");
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
    LOGCATE("GLUtils::DeleteProgram");
    if (program) {
        glUseProgram(0);
        glDeleteProgram(program);
        program = 0;
    }
}

void CheckGLError(const char *pGLOperation) {
    for (GLint error = glGetError(); error; error = glGetError()) {
        LOGCATE("GLUtils::CheckGLError GL Operation %s() glError (0x%x)\n", pGLOperation, error);
    }
}

std::thread* thread;

void testParams(int age,const char * name,const char * title) {
    int count = 0;
    while (true){
        count++;
        LOGCATE("打印书的年龄:%d 名字:%s 标题:%s 当前次数:%d",age,name,title,count);
        if (count > 10){
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
    thread = new std::thread(testParams,1,"23232","adfasdfsfs");
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




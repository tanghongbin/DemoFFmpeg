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
#include <BaseRender.h>
#include <VideoRender.h>
#include <AudioRender.h>


extern "C" {
#include <libavformat/avformat.h>
#include <libavutil/imgutils.h>
}

JavaVM *GLUtilC::mVm = nullptr;

int64_t GLUtilC::master_audio_clock = 0;

void CheckGLError(const char *pGLOperation);

BaseRender *getRender(jint type);

GLuint LoadShader(GLenum shaderType, const char *pSource) {
    GLuint shader = 0;
    shader = glCreateShader(shaderType);
    if (shader) {
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

GLuint CreateProgram(const char *pVertexShaderSource, const char *pFragShaderSource,
                     GLuint &vertexShaderHandle, GLuint &fragShaderHandle) {
    GLuint program = 0;
    vertexShaderHandle = LoadShader(GL_VERTEX_SHADER, pVertexShaderSource);
    if (!vertexShaderHandle) return program;

    fragShaderHandle = LoadShader(GL_FRAGMENT_SHADER, pFragShaderSource);
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

void testParams(int age) {
    LOGCATE("打印书的年龄:%d 名字: 标题:", age);
}

void testLocalThread() {
    std::thread(testParams, 1);
}

const char *encodeYuvToImageUtils(const char *filePath) {

    const char *IN_VALID_RESULT = "";
    LOGCATE("打印yuv图片地址:%s",filePath);

    AVFormatContext *pFormatContext;
    AVOutputFormat *fmt;
    AVStream *video_st;
    AVCodecContext *pCodecCtx;
    AVCodec *pCodec;
    AVCodecParameters* avCodecParameters;

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

    av_register_all();
    pFormatContext = avformat_alloc_context();
    fmt = av_guess_format("mjpeg", NULL, NULL);
    pFormatContext->oformat = fmt;
    if (avio_open(&pFormatContext->pb, outfile, AVIO_FLAG_READ_WRITE) < 0) {
        LOGCATE("can't open file");
        return IN_VALID_RESULT;
    }
    video_st = avformat_new_stream(pFormatContext, 0);
    if (video_st == NULL) {
        LOGCATE("create new stream failed ");
        return IN_VALID_RESULT;
    }
    pCodec = avcodec_find_encoder(fmt->video_codec);
    if (!pCodec) {
        LOGCATE("can't find encoder");
        return IN_VALID_RESULT;
    }
    LOGCATE("prepare fill codec params");

    pCodecCtx = avcodec_alloc_context3(pCodec);
//    pCodecCtx->codec_id = fmt->video_codec;
//    pCodecCtx->codec_type = AVMEDIA_TYPE_VIDEO;
//    pCodecCtx->pix_fmt = AV_PIX_FMT_YUV420P;
//
//    pCodecCtx->width = width;
//    pCodecCtx->height = height;
//
//    pCodecCtx->time_base.num = 1;
//    pCodecCtx->time_base.den = 25;
//    if (avcodec_parameters_to_context(pCodecCtx, avCodecParameters) != 0) {
//        LOGCATE("avcodec_parameters_to_context failed");
//        return IN_VALID_RESULT;
//    }
//    av_dump_format(pFormatContext, 0, outfile, 1);

    if (avcodec_open2(pCodecCtx,pCodec,NULL) < 0){
        LOGCATE("open encoder failed");
        return IN_VALID_RESULT;
    }
    picture = av_frame_alloc();
    size = av_image_get_buffer_size(pCodecCtx->pix_fmt,pCodecCtx->width,pCodecCtx->height,1);
    picture_buf = (uint8_t *)av_malloc(size);
    if (!picture_buf){
        LOGCATE("picture_buf malloc failed");
        return IN_VALID_RESULT;
    }

    avpicture_fill(reinterpret_cast<AVPicture *>(picture), picture_buf, pCodecCtx->pix_fmt, pCodecCtx->width, pCodecCtx->height);

    avformat_write_header(pFormatContext,NULL);
    y_size = pCodecCtx->width * pCodecCtx->height;
    av_new_packet(pkt,y_size*3);
    //read yuv
    if (fread(picture_buf,1,y_size*3/2,in_file) < 0){
        LOGCATE("can't open input file");
        return IN_VALID_RESULT;
    }
    picture->data[0] = picture_buf;
    picture->data[0] = picture_buf + y_size;
    picture->data[0] = picture_buf + y_size*5/4;

    // encode
    ret = avcodec_encode_video2(pCodecCtx,pkt,picture,&got_picture);
    if (ret < 0) {
        LOGCATE("encode error");
        return IN_VALID_RESULT;
    }
    if (got_picture == 1){
        pkt->stream_index = video_st->index;
        ret = av_write_frame(pFormatContext,pkt);
    }

    av_free_packet(pkt);
    av_write_trailer(pFormatContext);

    LOGCATE("encode successful");
    if (video_st){
        avcodec_close(pCodecCtx);
        av_free((picture));
        av_free(picture_buf);
    }
    avio_close(pFormatContext->pb);
    avformat_free_context(pFormatContext);
    fclose(in_file);

    return outfile;
}


void createThreadForPlay(JNIEnv *jniEnv, _jstring *url, _jobject *pJobject, jint type) {

    LOGCATE("thread start success");
    const char *localUrl = jniEnv->GetStringUTFChars(url, 0);
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

    LOGCATE("println all duration:%jd", mFormatContext->duration);

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
    baseRender->init(decode_context, jniEnv, pJobject);

    //9.解码循环
    while (av_read_frame(mFormatContext, packet) >= 0) { // 读取帧
        if (packet->stream_index == m_StreamIndex) {
            if (avcodec_send_packet(decode_context, packet) != 0) { // 视频解码
                return;
            }
            baseRender->eachPacket(packet, decode_context);
            while (avcodec_receive_frame(decode_context, frame) == 0) {
                //获取到 m_Frame 解码数据，在这里进行格式转换，然后进行渲染，下一节介绍 ANativeWindow 渲染过程
                baseRender->draw_frame(decode_context, frame, jniEnv, pJobject);
            }
        }
        av_packet_unref(packet);
    }


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

BaseRender *getRender(jint type) {
    if (type == 1) return new AudioRender();
    return new VideoRender;
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




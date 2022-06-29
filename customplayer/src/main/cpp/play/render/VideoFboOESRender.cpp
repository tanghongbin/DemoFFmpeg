//
// Created by Admin on 2021/5/13.
//

#include "../../play_header/render/VideoFboOESRender.h"
#include "../../play_header/model/shader.h"
#include "../../play_header/utils/CustomGLUtils.h"
#include <GLES3/gl3ext.h>
#include <EGL/egl.h>
#include <EGL/eglext.h>
#include <android/hardware_buffer.h>
#include "utils/utils.h"
#include <GLES2/gl2ext.h>

extern "C" {
#include <libyuv/convert.h>
#include <libyuv/scale.h>
#include <libyuv/scale_argb.h>
}


void VideoFboOESRender::Init(){
    shader = new Shader(readGLSLStrFromFile("fboplay/vetexOES.glsl").c_str(),
            readGLSLStrFromFile("fboplay/fragmentOES.glsl").c_str());
    rgbToYuvShader = new Shader(readGLSLStrFromFile("fboplay/vetex.glsl").c_str(),
                        readGLSLStrFromFile("fboplay/fragmentRgbToYuv420.glsl").c_str());
    rgbShader = new Shader(readGLSLStrFromFile("fboplay/vetex.glsl").c_str(),
                           readGLSLStrFromFile("fboplay/fragmentRGBA.glsl").c_str());
    GLfloat vetex[] = {
            -1.0f, -1.0f, 0.0f,  0.0f, 1.0f, // 0
            1.0f, -1.0f, 0.0f,  1.0f, 1.0f, // 1
            1.0f,  1.0f, 0.0f,  1.0f, 0.0f, // 2
            -1.0f, -1.0f, 0.0f,  0.0f, 1.0f, // 0
            1.0f,  1.0f, 0.0f,  1.0f, 0.0f, // 2
            -1.0f,  1.0f, 0.0f,  0.0f, 0.0f, // 3
    };

    GLfloat vetexReverse[] = {
            -1.0f, -1.0f, 0.0f,  0.0f, 0.0f, // 0
            1.0f, -1.0f, 0.0f,  1.0f, 0.0f, // 1
            1.0f,  1.0f, 0.0f,  1.0f, 1.0f, // 2
            -1.0f, -1.0f, 0.0f,  0.0f, 0.0f, // 0
            1.0f,  1.0f, 0.0f,  1.0f, 1.0f, // 2
            -1.0f,  1.0f, 0.0f,  0.0f, 1.0f, // 3
    };
    glGenBuffers(4,vboIds);
    glGenVertexArrays(2,vaoIds);

    glBindBuffer(GL_ARRAY_BUFFER,vboIds[0]);
    glBufferData(GL_ARRAY_BUFFER,sizeof(vetex),vetex,GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER,vboIds[1]);
    glBufferData(GL_ARRAY_BUFFER,sizeof(vetexReverse),vetexReverse,GL_STATIC_DRAW);

    glBindVertexArray(vaoIds[0]);
    glBindBuffer(GL_ARRAY_BUFFER,vboIds[0]);
    glVertexAttribPointer(0,3,GL_FLOAT,GL_FALSE,5 * sizeof(GL_FLOAT),(void *)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1,2,GL_FLOAT,GL_FALSE,5 * sizeof(GL_FLOAT),(void *)(3 * sizeof(GLfloat)));
    glEnableVertexAttribArray(1);
    glBindVertexArray(0);
    glBindVertexArray(vaoIds[1]);
    glBindBuffer(GL_ARRAY_BUFFER,vboIds[1]);
    glVertexAttribPointer(0,3,GL_FLOAT,GL_FALSE,5 * sizeof(GL_FLOAT),(void *)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1,2,GL_FLOAT,GL_FALSE,5 * sizeof(GL_FLOAT),(void *)(3 * sizeof(GLfloat)));
    glEnableVertexAttribArray(1);
    glBindVertexArray(0);

    //extTexture  用用扩展纹理
    glGenTextures(1,&extTexture);
    glBindTexture(GL_TEXTURE_EXTERNAL_OES,extTexture);
    glTexParameteri(GL_TEXTURE_EXTERNAL_OES, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_EXTERNAL_OES, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameterf(GL_TEXTURE_EXTERNAL_OES, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameterf(GL_TEXTURE_EXTERNAL_OES, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glBindTexture(GL_TEXTURE_EXTERNAL_OES,0);
    MsgLoopHelper::sendMsg(Message::obtain(JNI_COMMUNICATE_TYPE_OES_CREATE_SUCCESS,extTexture,0));
}

void VideoFboOESRender::createRgbaFbo() {
    if (fboRgbaId != 0) return;
    glGenTextures(1, &fboRgbaTextureId);
    glBindTexture(GL_TEXTURE_2D, fboRgbaTextureId);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, VIDEO_W, VIDEO_H, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
    glBindTexture(GL_TEXTURE_2D,0);

    glGenFramebuffers(1,&fboRgbaId);
    glBindFramebuffer(GL_FRAMEBUFFER, fboRgbaId);
    glBindTexture(GL_TEXTURE_2D, fboRgbaTextureId);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, fboRgbaTextureId, 0);
    LOGCATE("log width:%d height:%d",VIDEO_W,VIDEO_H);
    GLenum  result;
    if ((result = glCheckFramebufferStatus(GL_FRAMEBUFFER)) != GL_FRAMEBUFFER_COMPLETE){
        LOGCATE("log frame buffer is not complete %.2x",result);
        glCheckError("glCheckFramebufferStatus");
    } else {
        LOGCATE("create buffer complete");
    }
    glBindTexture(GL_TEXTURE_2D,0);
    glBindFramebuffer(GL_FRAMEBUFFER,0);
}

void VideoFboOESRender::createYuvFbo() {
    if (fboYuv420Id != 0) return;
    glGenTextures(1, &fboYuv420TextureId);
    glBindTexture(GL_TEXTURE_2D, fboYuv420TextureId);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, VIDEO_W/4, VIDEO_H*1.5, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
    glBindTexture(GL_TEXTURE_2D,0);
    glGenFramebuffers(1,&fboYuv420Id);
    glBindFramebuffer(GL_FRAMEBUFFER, fboYuv420Id);
    glBindTexture(GL_TEXTURE_2D, fboYuv420TextureId);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, fboYuv420TextureId, 0);
    LOGCATE("log width:%d height:%d",VIDEO_W,VIDEO_H);
    GLenum  result;
    if ((result = glCheckFramebufferStatus(GL_FRAMEBUFFER)) != GL_FRAMEBUFFER_COMPLETE){
        LOGCATE("log frame buffer is not complete %.2x",result);
        glCheckError("glCheckFramebufferStatus");
    } else {
        LOGCATE("create buffer complete");
    }
    glBindTexture(GL_TEXTURE_2D,0);
    glBindFramebuffer(GL_FRAMEBUFFER,0);
}

void VideoFboOESRender::DrawFrame() {
//    LOGCATE("im still draw");

    if (!shader || shader->ID == 0) return;
//    glPixelStorei(GL_UNPACK_ALIGNMENT,1);
    shader->use();


    glBindFramebuffer(GL_FRAMEBUFFER, fboRgbaId);
//    int64_t startsss = GetSysCurrentTime();
    drawFboTexture();
//    readRgbaImagePixel();
    glBindFramebuffer(GL_FRAMEBUFFER,0);


    // 将rgba绘制到yuv纹理中
    glBindFramebuffer(GL_FRAMEBUFFER, fboYuv420Id);
    rgbToYuvShader->use();
////    int64_t startsss = GetSysCurrentTime();
    glPixelStorei(GL_UNPACK_ALIGNMENT,1);
    drawYuv420pTexture();
    readYuvImagePixel();
    glBindFramebuffer(GL_FRAMEBUFFER,0);


    rgbShader->use();
//    shader->use();
    drawNormalRGBAImage();
//    LOGCATE("total fbo time:%lld",GetSysCurrentTime() - startsss);

}

/***
 * 通过fbo缩小纹理尺寸，搞成720x1280的
 */
void VideoFboOESRender::drawFboTexture() {
    glViewport(0, 0, VIDEO_W, VIDEO_H);
    glClearColor(1.0,1.0,1.0,1.0);
    glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
    // upload texture
//    LOGCATE("upload data success DrawFrame format:%d",nativeOpenGlImage.format);
// draw yuv420pdata

    GLuint currentVaoId = vaoIds[0];
    glBindVertexArray(currentVaoId);
    shader->setMat4("model", glm::mat4(1.0));
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_EXTERNAL_OES, extTexture);
    shader->setInt("yuvTexSampler", 0);
    shader->setMat4("uTextureMatrix",oesMatrix);
    glDrawArrays(GL_TRIANGLES,0,6);
    glBindVertexArray(0);
    glBindTexture(GL_TEXTURE_EXTERNAL_OES,0);
}

void VideoFboOESRender::readYuvImagePixel() {
    uint8_t *pBuffer = new uint8_t[VIDEO_W * VIDEO_H*3/2];

    auto* nativeImage = new NativeOpenGLImage;
    nativeImage->width = VIDEO_W;
    nativeImage->height = VIDEO_H;
    nativeImage->format = IMAGE_FORMAT_I420;
    nativeImage->ppPlane[0] = pBuffer;
    nativeImage->ppPlane[1] = pBuffer + nativeImage->width * nativeImage->height;
    nativeImage->ppPlane[2] = nativeImage->ppPlane[1] + nativeImage->width * nativeImage->height / 4;
    int64_t startTime = GetSysCurrentTime();
    glReadPixels(0, 0, VIDEO_W/4, VIDEO_H*1.5, GL_RGBA, GL_UNSIGNED_BYTE, pBuffer);
    readPixelCall(3,nativeImage);
//保存 I420 格式的 YUV 图片
//    std::string path = "/storage/emulated/0/ffmpegtest/filterImg";
//    NativeOpenGLImageUtil::DumpNativeImage(nativeImage, path.c_str(), "RGB2I420");
//    NativeOpenGLImageUtil::FreeNativeImage(nativeImage);
//    delete nativeImage;
}

void VideoFboOESRender::readRgbaImagePixel() {
    uint8_t *pBuffer = new uint8_t[VIDEO_W * VIDEO_H * 3 / 2];

    NativeOpenGLImage nativeImage;
    nativeImage.width = VIDEO_W;
    nativeImage.height = VIDEO_H;
    nativeImage.format = IMAGE_FORMAT_I420;
    nativeImage.ppPlane[0] = pBuffer;
    int64_t startTime = GetSysCurrentTime();
    glReadPixels(0, 0, VIDEO_W/4, VIDEO_H*1.5, GL_RGBA, GL_UNSIGNED_BYTE, pBuffer);
//    readPixelCall(3,openGlImage);
//保存 I420 格式的 YUV 图片
    std::string path = "/storage/emulated/0/ffmpegtest/filterImg";
    NativeOpenGLImageUtil::DumpNativeImage(&nativeImage, path.c_str(), "RGB2I420");
}

void VideoFboOESRender::UpdateOESMartix(float *pDouble){
    oesMatrix = pDouble;
}

void VideoFboOESRender::drawNormalImage() {
    // draw rgba data
    GLuint currentvaoId = vaoIds[1];
    glViewport(0, 0, renderWidth, renderHeight);
    glClearColor(1.0,1.0,1.0,1.0);
    glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
//    // 画camera 数据
    glBindVertexArray(currentvaoId);
    shader->setMat4("model", glm::mat4(1.0));
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_EXTERNAL_OES, extTexture);
    shader->setInt("yuvTexSampler", 0);
    shader->setMat4("uTextureMatrix",oesMatrix);
    glDrawArrays(GL_TRIANGLES,0,6);
    glBindVertexArray(0);
    glBindTexture(GL_TEXTURE_EXTERNAL_OES,0);
//    drawLogo(currentvaoId);
}

void VideoFboOESRender::drawNormalRGBAImage() {
    // draw rgba data
    GLuint currentvaoId = vaoIds[0];
    glViewport(0, 0, renderWidth, renderHeight);
    glClearColor(1.0,1.0,1.0,1.0);
    glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
//    // 画camera 数据
    glBindVertexArray(currentvaoId);
    rgbShader->setMat4("model", glm::mat4(1.0));
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, fboRgbaTextureId);
    rgbShader->setInt("s_TextureMap", 0);
    glDrawArrays(GL_TRIANGLES,0,6);
    glBindVertexArray(0);
    glBindTexture(GL_TEXTURE_2D,0);
//    drawLogo(currentvaoId);
}

/***
 * 通过将720x1280的纹理画到yuv420的纹理中间去
 */
void VideoFboOESRender::drawYuv420pTexture() {
    glViewport(0, 0, VIDEO_W/4, VIDEO_H*1.5);
    glClearColor(1.0,1.0,1.0,1.0);
    glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
    // upload texture
//    LOGCATE("upload data success DrawFrame format:%d",nativeOpenGlImage.format);
// draw yuv420pdata

    GLuint currentVaoId = vaoIds[1];
    glBindVertexArray(currentVaoId);
    rgbToYuvShader->setMat4("model", glm::mat4(1.0));
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, fboRgbaTextureId);
    rgbToYuvShader->setInt("s_TextureMap", 0);
    float value = (float)1.0/(float)VIDEO_W;
    rgbToYuvShader->setFloat("u_Offset",value);
    rgbToYuvShader->setVec2("u_ImgSize",VIDEO_W,VIDEO_H);
    glDrawArrays(GL_TRIANGLES,0,6);
    glBindVertexArray(0);
    glBindTexture(GL_TEXTURE_2D,0);
}

void VideoFboOESRender::Destroy() {
    std::lock_guard<std::mutex> lockGuard(renderMutex);
    shader->Destroy();

    glDeleteTextures(1,&fboYuv420TextureId);
    glDeleteTextures(1,&fboRgbaTextureId);
    glDeleteTextures(1,&extTexture);
    glDeleteBuffers(4,vboIds);
    glDeleteFramebuffers(1,&fboYuv420Id);
    glDeleteFramebuffers(1,&fboRgbaId);
    glDeleteVertexArrays(2,vaoIds);
//    NativeOpenGLImageUtil::FreeNativeImage(&nativeOpenGlImage);
    LOGCATE("delete video render is success");
}

VideoFboOESRender::~VideoFboOESRender(){
    if (rgbToYuvShader) {
        rgbToYuvShader->Destroy();
        delete rgbToYuvShader;
        rgbToYuvShader = 0;
    }
    if (rgbShader) {
        rgbShader->Destroy();
        delete rgbShader;
        rgbShader = 0;
    }
LOGCATE("VideoFboOESRender is destroyed");
}

void VideoFboOESRender::OnSurfaceChanged(int windowW,int windowH){
    renderWidth = windowW;
    renderHeight = windowH;
    createYuvFbo();
    createRgbaFbo();
}







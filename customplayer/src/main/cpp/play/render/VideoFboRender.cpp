//
// Created by Admin on 2021/5/13.
//

#include "../../play_header/render/VideoFboRender.h"
#include "../../play_header/model/shader.h"
#include "../../play_header/utils/CustomGLUtils.h"
#include <GLES3/gl3ext.h>
#include <EGL/egl.h>
#include <EGL/eglext.h>
#include <android/hardware_buffer.h>

extern "C" {
#include <libyuv/convert.h>
#include <libyuv/scale.h>
#include <libyuv/scale_argb.h>
}


void VideoFboRender::Init(){
    shader = new Shader(readGLSLStrFromFile("fboplay/vetex.glsl").c_str(),
            readGLSLStrFromFile("fboplay/fragment.glsl").c_str());

    GLfloat vetex[] = {
            -1.0f, -1.0f, 0.0f,  0.0f, 1.0f,
            1.0f, -1.0f, 0.0f,  1.0f, 1.0f,
            1.0f,  1.0f, 0.0f,  1.0f, 0.0f,
            1.0f,  1.0f, 0.0f,  1.0f, 0.0f,
            -1.0f,  1.0f, 0.0f,  0.0f, 0.0f,
            -1.0f, -1.0f, 0.0f,  0.0f, 1.0f,
    };

    GLfloat vetexReverse[] = {
            -1.0f, -1.0f, 0.0f,  0.0f, 0.0f,
            1.0f, -1.0f, 0.0f,  1.0f, 0.0f,
            1.0f,  1.0f, 0.0f,  1.0f, 1.0f,
            1.0f,  1.0f, 0.0f,  1.0f, 1.0f,
            -1.0f,  1.0f, 0.0f,  0.0f, 1.0f,
            -1.0f, -1.0f, 0.0f,  0.0f, 0.0f,
    };
    glGenBuffers(4,vboIds);
    glGenVertexArrays(2,vaoIds);
    glGenTextures(4,textures);
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
    for (int i = 0; i < TEXTURE_FBO_NUM - 1; ++i) {
        glBindTexture(GL_TEXTURE_2D,textures[i]);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glBindTexture(GL_TEXTURE_2D,0);
    }
    LoadImageInfo imageInfo;
    imageInfo.loadImage("/storage/emulated/0/ffmpegtest/filterImg/slight.jpg");
    glBindTexture(GL_TEXTURE_2D,textures[3]);
    imageInfo.setupNormalSetting();
    imageInfo.uploadImageTex2D();
    glBindTexture(GL_TEXTURE_2D,0);
}

void VideoFboRender::createPbo() {
    glGenBuffers(2,pboIds);
    int size = renderWidth * renderHeight * 4;
    glBindBuffer(GL_PIXEL_PACK_BUFFER,pboIds[0]);
    glBufferData(GL_PIXEL_PACK_BUFFER,size, nullptr,GL_STATIC_DRAW);
    glBindBuffer(GL_PIXEL_PACK_BUFFER,pboIds[1]);
    glBufferData(GL_PIXEL_PACK_BUFFER,size, nullptr,GL_STATIC_DRAW);
    glBindBuffer(GL_PIXEL_PACK_BUFFER,0);
}

void VideoFboRender::createFbo() {
    if (fboId != 0) return;
    glGenTextures(1, &fboTextureId);
    glBindTexture(GL_TEXTURE_2D, fboId);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glBindTexture(GL_TEXTURE_2D,0);
    glGenFramebuffers(1,&fboId);
    glBindFramebuffer(GL_FRAMEBUFFER, fboId);
    glBindTexture(GL_TEXTURE_2D, fboTextureId);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, renderWidth, renderHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, fboTextureId, 0);
    LOGCATE("log width:%d height:%d",renderWidth,renderHeight);
    GLenum  result;
    if ((result = glCheckFramebufferStatus(GL_FRAMEBUFFER)) != GL_FRAMEBUFFER_COMPLETE){
        LOGCATE("log frame buffer is not complete %.2x",result);
        glCheckError("glCheckFramebufferStatus");
    }
    glBindTexture(GL_TEXTURE_2D,0);
    glBindFramebuffer(GL_FRAMEBUFFER,0);
}

void VideoFboRender::DrawFrame() {
//    LOGCATE("im still draw");

    if (!shader || shader->ID == 0) return;

    std::unique_lock<std::mutex> uniqueLock(renderMutex,std::defer_lock);
    uniqueLock.lock();
    if (nativeOpenGlImage.width != 0 && nativeOpenGlImage.height != 0){
        glBindTexture(GL_TEXTURE_2D, textures[0]);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_LUMINANCE, nativeOpenGlImage.width,
                     nativeOpenGlImage.height, 0, GL_LUMINANCE, GL_UNSIGNED_BYTE,
                     nativeOpenGlImage.ppPlane[0]);
        glBindTexture(GL_TEXTURE_2D, GL_NONE);

        //update U plane data
        glBindTexture(GL_TEXTURE_2D, textures[1]);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_LUMINANCE, nativeOpenGlImage.width >> 1,
                     nativeOpenGlImage.height >> 1, 0, GL_LUMINANCE, GL_UNSIGNED_BYTE,
                     nativeOpenGlImage.ppPlane[1]);
        glBindTexture(GL_TEXTURE_2D, GL_NONE);

        //update V plane data
        glBindTexture(GL_TEXTURE_2D, textures[2]);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_LUMINANCE, nativeOpenGlImage.width >> 1,
                     nativeOpenGlImage.height >> 1, 0, GL_LUMINANCE, GL_UNSIGNED_BYTE,
                     nativeOpenGlImage.ppPlane[2]);
        glBindTexture(GL_TEXTURE_2D, GL_NONE);
    } else {
        uniqueLock.unlock();
        return;
    }
    uniqueLock.unlock();

    glBindFramebuffer(GL_FRAMEBUFFER,fboId);
    int64_t startsss = GetSysCurrentTime();
    drawNormalImage(true);
    readImagePixel();
//    readImagePixelHardBuffer();
    glBindFramebuffer(GL_FRAMEBUFFER,0);

    drawNormalImage(false);
//    LOGCATE("total fbo time:%lld",GetSysCurrentTime() - startsss);

}

void VideoFboRender::readImagePixel() {
    auto * rgbaData = new uint8_t [renderWidth * renderHeight * 4];
    glReadPixels(0, 0, renderWidth, renderHeight, GL_RGBA, GL_UNSIGNED_BYTE, rgbaData);
    readPixelCall(rgbaData);
}

void VideoFboRender::drawNormalImage(bool isFboRender) {
    glViewport(0, 0, renderWidth, renderHeight);
    glClearColor(0.0,0.0,0.0,1.0);
    glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
    // upload texture
//    LOGCATE("upload data success DrawFrame format:%d",nativeOpenGlImage.format);
// draw
    shader->use();
    GLuint currentFboId = isFboRender ? vaoIds[1] : vaoIds[0];
    glBindVertexArray(currentFboId);
    shader->setMat4("model", glm::mat4(1.0));
    shader->setInt("samplerType", 2);
    for (int i = 0; i < TEXTURE_FBO_NUM; ++i) {
        glActiveTexture(GL_TEXTURE0 + i);
        glBindTexture(GL_TEXTURE_2D, textures[i]);
        char samplerName[64] = {0};
        sprintf(samplerName, "s_TextureMap%d", i);
        shader->setInt(samplerName, i);
    }
    glDrawArrays(GL_TRIANGLES,0,6);

    glBindVertexArray(currentFboId);
    glm::mat4 currentModel = glm::mat4(1.0);
    currentModel = glm::scale(currentModel,glm::vec3 (0.1f));
    if (isFboRender){
        currentModel = glm::translate(currentModel,glm::vec3(8.5f,-8.6f,0.0f));
    } else {
        currentModel = glm::translate(currentModel,glm::vec3(8.5f,8.6f,0.0f));
    }
    shader->setMat4("model", currentModel);
    shader->setInt("samplerType", 1);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, textures[3]);
    char samplerName[64] = {0};
    sprintf(samplerName, "s_TextureMap%d", 0);
    shader->setInt(samplerName, 0);
    glDrawArrays(GL_TRIANGLES,0,6);
}

void VideoFboRender::Destroy() {
    std::lock_guard<std::mutex> lockGuard(renderMutex);
    renderIsFinish = true;
    shader->Destroy();
    glDeleteTextures(1,&fboTextureId);
    glDeleteBuffers(4,vboIds);
    glDeleteFramebuffers(1,&fboId);
    glDeleteVertexArrays(2,vaoIds);
    glDeleteTextures(4,textures);
    glDeleteBuffers(2,pboIds);
    NativeOpenGLImageUtil::FreeNativeImage(&nativeOpenGlImage);
    LOGCATE("delete video render is success");
}

VideoFboRender::~VideoFboRender(){
    renderIsFinish = true;
LOGCATE("VideoFboRender is destroyed");
}

void VideoFboRender::copyImage(NativeOpenGLImage *srcImage){
//    LOGCATE("log image params width:%d height:%d format:%d data:%p",
//            srcImage->width,srcImage->height,srcImage->format,srcImage->ppPlane[0]);
//    LOGCATE("prepare lock and copyImage");
    std::lock_guard<std::mutex> lockGuard(renderMutex);
    if (renderIsFinish) return;
    if (nativeOpenGlImage.ppPlane[0] == nullptr){
        nativeOpenGlImage.width = srcImage->width;
        nativeOpenGlImage.height = srcImage->height;
        nativeOpenGlImage.format = srcImage->format;
        NativeOpenGLImageUtil::AllocNativeImage(&nativeOpenGlImage);
    }
    NativeOpenGLImageUtil::CopyNativeImage(srcImage,&nativeOpenGlImage);
}


void VideoFboRender::OnSurfaceChanged(int windowW,int windowH){
    renderWidth = windowW;
    renderHeight = windowH;
    createFbo();
    createPbo();
}

void VideoFboRender::readImagePixelHardBuffer() {
    // OUR parameters that we will set and give it to AHardwareBuffer
//    AHardwareBuffer_Desc usage;
//
//// filling in the usage for HardwareBuffer
//    usage.format = AHARDWAREBUFFER_FORMAT_R8G8B8A8_UNORM;
//    usage.height = renderHeight;
//    usage.width = renderWidth;
//    usage.layers = 1;
//    usage.rfu0 = 0;
//    usage.rfu1 = 0;
//    usage.stride = 10;
//    usage.usage = AHARDWAREBUFFER_USAGE_CPU_READ_OFTEN | AHARDWAREBUFFER_USAGE_CPU_WRITE_NEVER
//                  | AHARDWAREBUFFER_USAGE_GPU_COLOR_OUTPUT;
//    // create GraphicBuffer
//    AHardwareBuffer* graphicBuf;
//    AHardwareBuffer_allocate(&usage, &graphicBuf); // it's worth to check the return code
//
//// ACTUAL parameters of the AHardwareBuffer which it reports
//    AHardwareBuffer_Desc usage1;
//
//// for stride, see below
//    AHardwareBuffer_describe(graphicBuf, &usage1);
//
//// get the native buffer
//    EGLClientBuffer clientBuf = eglGetNativeClientBufferANDROID(graphicBuf);
//
//// obtaining the EGL display
//    EGLDisplay disp = eglGetDisplay(EGL_DEFAULT_DISPLAY);
//
//// specifying the image attributes
//    EGLint eglImageAttributes[] = {EGL_IMAGE_PRESERVED_KHR, EGL_TRUE, EGL_NONE};
//
//    EGLImageKHR imageEGL = eglCreateImageKHR(disp, EGL_NO_CONTEXT, EGL_NATIVE_BUFFER_ANDROID, clientBuf, eglImageAttributes);
//
//    // attaching an EGLImage to OUTPUT texture
//    glEGLImageTargetTexture2DOES(GL_TEXTURE_2D, imageEGL);
//
//    // pointer for reading and writing texture data
//    void *readPtr, *writePtr;
//
//// locking the buffer
//    graphicBuf->lock(GraphicBuffer::USAGE_SW_READ_OFTEN, &readPtr);
//
//// setting the write pointer
//    writePtr = <set to a valid memory area, like malloc(_YOUR_SIZE_)>
//
//                                                 // obtaining the stride (for me it was always = width)
//                                                 int stride = graphicBuf->getStride();
//
//// loop over texture rows
//    for (int row = 0; row < height; row++) {
//        // copying, 4 = 4 channels RGBA because of the format above
//        memcpy(writePtr, readPtr, width * 4);
//
//        // adding stride * 4 to read pointer
//        readPtr = (void *)(int(readPtr) + stride * 4);
//
//        // adding width * 4 to write pointer
//        writePtr = (void *)(int(writePtr) + width * 4);
//    }
//
//// NOW data is in writePtr memory
//
//// unlocking the buffer
//    graphicBuf->unlock();
}



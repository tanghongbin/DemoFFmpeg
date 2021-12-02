//
// Created by Admin on 2021/5/13.
//

#include "../../play_header/render/VideoSpecialEffortsRender.h"
#include "../../play_header/model/shader.h"
#include "../../play_header/utils/CustomGLUtils.h"


void VideoSpecialEffortsRender::Init(){
    shader = new Shader(readGLSLStrFromFile("videospecialefforts/vetex.glsl").c_str(),
            readGLSLStrFromFile("videospecialefforts/fragment.glsl").c_str());

    GLfloat vetex[] = {
            -1.0f, -1.0f, 0.0f,  0.0f, 1.0f,
            1.0f, -1.0f, 0.0f,  1.0f, 1.0f,
            1.0f,  1.0f, 0.0f,  1.0f, 0.0f,
            1.0f,  1.0f, 0.0f,  1.0f, 0.0f,
            -1.0f,  1.0f, 0.0f,  0.0f, 0.0f,
            -1.0f, -1.0f, 0.0f,  0.0f, 1.0f,
    };
    glGenBuffers(4,vboIds);
    glGenVertexArrays(2,vaoIds);
    glGenTextures(4,textures);
    glBindBuffer(GL_ARRAY_BUFFER,vboIds[0]);
    glBufferData(GL_ARRAY_BUFFER,sizeof(vetex),vetex,GL_STATIC_DRAW);

    glBindVertexArray(vaoIds[0]);
    glBindBuffer(GL_ARRAY_BUFFER,vboIds[0]);
    glVertexAttribPointer(0,3,GL_FLOAT,GL_FALSE,5 * sizeof(GL_FLOAT),(void *)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1,2,GL_FLOAT,GL_FALSE,5 * sizeof(GL_FLOAT),(void *)(3 * sizeof(GLfloat)));
    glEnableVertexAttribArray(1);
    glBindVertexArray(0);
    LoadImageInfo imageInfo;
    imageInfo.loadImage("/storage/emulated/0/ffmpegtest/filterImg/test_box.png");
    for (int i = 0; i < TEXTURE_NUM; ++i) {
        glBindTexture(GL_TEXTURE_2D,textures[i]);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glBindTexture(GL_TEXTURE_2D,0);
    }

}

void VideoSpecialEffortsRender::DrawFrame() {
//    LOGCATE("im still draw");

    if (!shader || shader->ID == 0) return;
    glViewport(widthOffset,heightOffset,renderWidth,renderHeight);
    glClearColor(0.0,0.0,0.0,1.0);
    glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
    // upload texture
    drawImage(&nativeOpenGlImage, 1.0f);
    drawImage(&specialEffortsImage, 0.3f);
    glDisable(GL_BLEND);
    if (readResultImg.ppPlane[0] == nullptr) {
        NativeOpenGLImageUtil::AllocNativeImage(&readResultImg);
    }
    long long int start = GetSysCurrentTime();
    glReadPixels(0,0,renderWidth,renderHeight,GL_RGBA,GL_UNSIGNED_BYTE,readResultImg.ppPlane[0]);
    if (onReadPixelListener) onReadPixelListener->readPixelResult(&readResultImg);
    LOGCATE("总共读取时间:%lld",start);
}

void VideoSpecialEffortsRender::drawImage(NativeOpenGLImage *targetImg, float alpha) {
//    LOGCATE("upload data success DrawFrame format:%d",nativeOpenGlImage.format);
// draw
    std::__ndk1::unique_lock<std::mutex> uniqueLock(renderMutex, std::defer_lock);
    uniqueLock.lock();
    if (targetImg -> width != 0 && targetImg -> height != 0 && targetImg->ppPlane[0]){
        glBindTexture(GL_TEXTURE_2D, textures[0]);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_LUMINANCE, targetImg -> width,
                     targetImg -> height, 0, GL_LUMINANCE, GL_UNSIGNED_BYTE,
                     targetImg -> ppPlane[0]);
        glBindTexture(GL_TEXTURE_2D, GL_NONE);

        //update U plane data
        glBindTexture(GL_TEXTURE_2D, textures[1]);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_LUMINANCE, targetImg -> width >> 1,
                     targetImg -> height >> 1, 0, GL_LUMINANCE, GL_UNSIGNED_BYTE,
                     targetImg -> ppPlane[1]);
        glBindTexture(GL_TEXTURE_2D, GL_NONE);

        //update V plane data
        glBindTexture(GL_TEXTURE_2D, textures[2]);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_LUMINANCE, targetImg -> width >> 1,
                     targetImg -> height >> 1, 0, GL_LUMINANCE, GL_UNSIGNED_BYTE,
                     targetImg -> ppPlane[2]);
        glBindTexture(GL_TEXTURE_2D, GL_NONE);
    } else {
        uniqueLock.unlock();
        return;
    }
    uniqueLock.unlock();
    shader->use();
    glBindVertexArray(vaoIds[0]);
    for (int i = 0; i < TEXTURE_NUM; ++i) {
        glActiveTexture(GL_TEXTURE0 + i);
        glBindTexture(GL_TEXTURE_2D, textures[i]);
        char samplerName[64] = {0};
        sprintf(samplerName, "s_TextureMap%d", i);
        shader->setInt(samplerName, i);
    }
    shader->setFloat("s_alpha",alpha);
    glDrawArrays(GL_TRIANGLES,0,6);
}

void VideoSpecialEffortsRender::Destroy() {
    std::lock_guard<std::mutex> lockGuard(renderMutex);
    renderIsFinish = true;
    shader->Destroy();
    glDeleteBuffers(4,vboIds);
    glDeleteVertexArrays(2,vaoIds);
    glDeleteTextures(4,textures);
    if (specialEffortsImage.ppPlane[0]) NativeOpenGLImageUtil::FreeNativeImage(&specialEffortsImage);
    NativeOpenGLImageUtil::FreeNativeImage(&nativeOpenGlImage);
    NativeOpenGLImageUtil::FreeNativeImage(&readResultImg);
    LOGCATE("delete video render is success");
}

VideoSpecialEffortsRender::~VideoSpecialEffortsRender(){
    renderIsFinish = true;
LOGCATE("VideoSpecialEffortsRender is destroyed");
}

void VideoSpecialEffortsRender::copyImage(NativeOpenGLImage *srcImage){
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

void VideoSpecialEffortsRender::copySpecialEffortsImage(NativeOpenGLImage *special){
    std::lock_guard<std::mutex> lockGuard(renderMutex);
    if (renderIsFinish) return;
    if (specialEffortsImage.ppPlane[0] == nullptr){
        specialEffortsImage.width = special->width;
        specialEffortsImage.height = special->height;
        specialEffortsImage.format = special->format;
        NativeOpenGLImageUtil::AllocNativeImage(&specialEffortsImage);
    }
    NativeOpenGLImageUtil::CopyNativeImage(special,&specialEffortsImage);
}

void VideoSpecialEffortsRender::destroySpecialEffortsImage(){
    std::lock_guard<std::mutex> lockGuard(renderMutex);
    if (specialEffortsImage.ppPlane[0]){
        NativeOpenGLImageUtil::FreeNativeImage(&specialEffortsImage);
    }
}

void VideoSpecialEffortsRender::OnRenderSizeChanged(int windowW,int windowH,int renderW,int renderH){
    if (windowW < windowH) {
        // 竖屏
        if (renderW > renderH) {
            widthOffset = 0;
            heightOffset = (windowH - renderH) / 2;
            renderWidth = renderW;
            renderHeight = renderH;
        } else {
            widthOffset = (windowW - renderW) / 2;
            heightOffset = 0;
            renderWidth = renderW;
            renderHeight = renderH;
        }
    } else {
        if (renderW < renderH) {
            widthOffset = 0;
            heightOffset = (windowH - renderH) / 2;
            renderWidth = renderW;
            renderHeight = renderH;
        } else {
            widthOffset = (windowW - renderW) / 2;
            heightOffset = 0;
            renderWidth = renderW;
            renderHeight = renderH;
        }
    }
    LOGCATE("log offsetW:%d offsetH:%d windowW:%d windowH:%d renderW:%d renderH:%d",widthOffset,heightOffset,
            windowW,windowH,renderWidth,renderHeight);
}
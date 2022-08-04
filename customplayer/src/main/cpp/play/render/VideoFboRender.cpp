//
// Created by Admin on 2021/5/13.
//



#include <render/VideoFboRender.h>

void VideoFboRender::Init(){
    shader = new Shader(readGLSLStrFromFile("fboplay/vetex.glsl").c_str(),
            readGLSLStrFromFile("fboplay/fragment.glsl").c_str());

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
    glGenTextures(4,textures);
    glGenTextures(4,filterTextures);
    glGenTextures(1,&lutTextureId);
    glGenTextures(1,&logoTextureId);

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

    glBindTexture(GL_TEXTURE_2D,lutTextureId);
    LoadImageInfo lutImageInfo;
//    lutImageInfo.loadImage("/storage/emulated/0/ffmpegtest/filterImg/lut_1.png");
    lutImageInfo.loadImage("/storage/emulated/0/ffmpegtest/filterImg/lut_2.jpeg");
//    lutImageInfo.loadImage("/storage/emulated/0/ffmpegtest/filterImg/lut_3.jpeg");
    lutImageInfo.setupNormalSetting();
    lutImageInfo.uploadImageTex2D();
    glBindTexture(GL_TEXTURE_2D,0);

    // bind logo texture
    glBindTexture(GL_TEXTURE_2D,logoTextureId);
    LoadImageInfo logoImgInfo;
    logoImgInfo.loadImage("/storage/emulated/0/ffmpegtest/filterImg/java.jpg");
    logoImgInfo.setupNormalSetting();
    logoImgInfo.uploadImageTex2D();


//    inputTextureHandles[0] = OpenGLUtils.loadTexture(context, "filter/brannan_blowout.png")
//    inputTextureHandles[1] = OpenGLUtils.loadTexture(context, "filter/overlaymap.png")
//    inputTextureHandles[2] = OpenGLUtils.loadTexture(context, "filter/amaromap.png")
    // load gpuimagefilter 尝试gpuimagefilter 滤镜
    const char* filterPaths [TEXTURE_FBO_NUM - 1]  = {
            getAssetsFileAbsolutePath("filter/brannan_blowout.png").c_str(),
            getAssetsFileAbsolutePath("filter/overlaymap.png").c_str(),
            getAssetsFileAbsolutePath("filter/amaromap.png").c_str(),
    };
    for (int i = 0; i < TEXTURE_FBO_NUM - 1; ++i) {
        glBindTexture(GL_TEXTURE_2D,filterTextures[i]);
        LoadImageInfo loadImageInfo;
        loadImageInfo.setupNormalSetting();
        loadImageInfo.loadImage(filterPaths[i]);
        loadImageInfo.uploadImageTex2D();
        glBindTexture(GL_TEXTURE_2D,0);
    }
}

void VideoFboRender::createPbo() {
    glGenBuffers(2,pboIds);
    int size = VIDEO_W * VIDEO_H * 4;
    glBindBuffer(GL_PIXEL_PACK_BUFFER,pboIds[0]);
    glBufferData(GL_PIXEL_PACK_BUFFER,size, nullptr,GL_STATIC_DRAW);
    glBindBuffer(GL_PIXEL_PACK_BUFFER,pboIds[1]);
    glBufferData(GL_PIXEL_PACK_BUFFER,size, nullptr,GL_STATIC_DRAW);
    glBindBuffer(GL_PIXEL_PACK_BUFFER,0);
}

void VideoFboRender::createFbo() {
    if (fboId != 0) return;
    glGenTextures(1, &fboTextureId);
    glBindTexture(GL_TEXTURE_2D, fboTextureId);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, VIDEO_W, VIDEO_H, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
    glBindTexture(GL_TEXTURE_2D,0);

    glGenTextures(1, &testRgbaTextureId);
    glBindTexture(GL_TEXTURE_2D, testRgbaTextureId);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glBindTexture(GL_TEXTURE_2D,0);


    glGenFramebuffers(1,&fboId);
    glBindFramebuffer(GL_FRAMEBUFFER, fboId);
    glBindTexture(GL_TEXTURE_2D, fboTextureId);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, fboTextureId, 0);
    GLenum  result;
    if ((result = glCheckFramebufferStatus(GL_FRAMEBUFFER)) != GL_FRAMEBUFFER_COMPLETE){
        LOGCATE("log frame buffer is not complete %.2x",result);
        glCheckError("glCheckFramebufferStatus");
    } else {
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

    glPixelStorei(GL_UNPACK_ALIGNMENT,1);
    shader->use();
    glBindFramebuffer(GL_FRAMEBUFFER,fboId);
//    int64_t startsss = GetSysCurrentTime();
    drawFboTexture();
//    readImagePixelByPbo();
    readImagePixel();
////    readImagePixelHardBuffer();
    glBindFramebuffer(GL_FRAMEBUFFER,0);

    drawNormalImage();
//    LOGCATE("total fbo time:%lld",GetSysCurrentTime() - startsss);

}

void VideoFboRender::readImagePixel() {
    auto * openGlImage = new NativeOpenGLImage;
    openGlImage->width = VIDEO_W;
    openGlImage->height = VIDEO_H;
    openGlImage->format = IMAGE_FORMAT_RGBA;
    NativeOpenGLImageUtil::AllocNativeImage(openGlImage);
    int64_t startTime = GetSysCurrentTime();
    glReadPixels(0, 0, VIDEO_W, VIDEO_H, GL_RGBA, GL_UNSIGNED_BYTE, openGlImage->ppPlane[0]);
//    LOGCATE("打印读取时间：%lld",GetSysCurrentTime() - startTime);
//    glBindTexture(GL_TEXTURE_2D,testRgbaTextureId);
//    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, VIDEO_W, VIDEO_H, 0, GL_RGBA, GL_UNSIGNED_BYTE, rgbaData);
//    glBindTexture(GL_TEXTURE_2D,0);
//    delete [] rgbaData;
    // 测试读取显示
    readPixelCall(3,openGlImage);
}

void VideoFboRender::drawNormalImage() {
    // draw rgba data
    GLuint currentvaoId = vaoIds[0];
    glViewport(0, 0, renderWidth, renderHeight);
    glClearColor(1.0,1.0,1.0,1.0);
    glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
//
//    // 画camera 数据
    glBindVertexArray(currentvaoId);
    shader->setMat4("model", glm::mat4(1.0));
    shader->setInt("samplerType", 1);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, fboTextureId);
    char samplerName[64] = {0};
    sprintf(samplerName, "s_TextureMap%d", 0);
    shader->setInt(samplerName, 0);
    glDrawArrays(GL_TRIANGLES,0,6);
    glBindVertexArray(0);
    glBindTexture(GL_TEXTURE_2D,0);

//    drawLogo(currentvaoId);
}


void VideoFboRender::drawFboTexture() {
    glViewport(0, 0, VIDEO_W, VIDEO_H);
    glClearColor(1.0,1.0,1.0,1.0);
    glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
    // upload texture
//    LOGCATE("upload data success DrawFrame format:%d",nativeOpenGlImage.format);
// draw yuv420pdata

    if (u_offset > 5) {
        u_offset = 0;
    }
    GLuint currentVaoId = vaoIds[1];
    glBindVertexArray(currentVaoId);
    shader->setMat4("model", glm::mat4(1.0));
    shader->setInt("samplerType", 2);
    shader->setInt("u_offset",u_offset);
    shader->setInt("fliterType",6);
    shader->setFloat("blurSize",5.0f);
    shader->setVec2("texSize",glm::vec2(VIDEO_W,VIDEO_H));
    shader->setVec2("blurCenter",glm::vec2(0.9));
    for (int i = 0; i < TEXTURE_FBO_NUM; ++i) {
        glActiveTexture(GL_TEXTURE0 + i);
        glBindTexture(GL_TEXTURE_2D, textures[i]);
        char samplerName[64] = {0};
        sprintf(samplerName, "s_TextureMap%d", i);
        shader->setInt(samplerName, i);
    }
    glActiveTexture(GL_TEXTURE4);
    glBindTexture(GL_TEXTURE_2D,lutTextureId);
    shader->setInt("s_LutTexture",4);
    // 把多个普通滤镜的图片传过去
    for (int i = 0; i < TEXTURE_FBO_NUM - 1; ++i) {
        glActiveTexture(GL_TEXTURE5 + i);
        glBindTexture(GL_TEXTURE_2D, filterTextures[i]);
        char filterName[64] = {0};
        sprintf(filterName, "s_Filter%d", i);
        shader->setInt(filterName, i + 5);
    }
    glDrawArrays(GL_TRIANGLES,0,6);
    glBindVertexArray(0);
    glBindTexture(GL_TEXTURE_2D,0);

    // 画logo
    drawLogo(currentVaoId);
    u_offset++;
}

void VideoFboRender::drawLogo(GLuint currentVaoId) {
    if (x_test_offset > 4.0) {
        x_test_offset = -4.0;
    }
    glBindVertexArray(currentVaoId);
    glm::mat4 currentModel = glm::mat4(1.0);
    currentModel = glm::scale(currentModel,glm::vec3 (0.2f));
    currentModel = glm::translate(currentModel,glm::vec3 (x_test_offset,x_test_offset,0.0f));
    shader->setMat4("model", currentModel);
    shader->setInt("samplerType", 1);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, textures[3]);
    char samplerName[64] = {0};
    sprintf(samplerName, "s_TextureMap%d", 0);
    shader->setInt(samplerName, 0);
    glDrawArrays(GL_TRIANGLES,0,6);
    glBindVertexArray(0);
    glBindTexture(GL_TEXTURE_2D,0);
    x_test_offset += 0.02;
}

void VideoFboRender::Destroy() {
    std::lock_guard<std::mutex> lockGuard(renderMutex);
    renderIsFinish = true;
    shader->Destroy();

    glDeleteTextures(4,textures);
    glDeleteTextures(4,filterTextures);
    glDeleteTextures(1,&fboTextureId);
    glDeleteTextures(1,&lutTextureId);
    glDeleteTextures(1,&logoTextureId);

    glDeleteBuffers(4,vboIds);
    glDeleteFramebuffers(1,&fboId);
    glDeleteVertexArrays(2,vaoIds);
    glDeleteBuffers(2,pboIds);
    NativeOpenGLImageUtil::FreeNativeImage(&nativeOpenGlImage);
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

void VideoFboRender::readImagePixelByPbo() {
    if (currentPboId == 0 || currentPboId == pboIds[1]){
        currentPboId = pboIds[0];
        nextPboId = pboIds[1];
    } else {
        currentPboId = pboIds[1];
        nextPboId = pboIds[0];
    }
    int rgb720Length = VIDEO_W * VIDEO_H * 4;
    glBindBuffer(GL_PIXEL_PACK_BUFFER,currentPboId);
    glReadPixels(0,0,VIDEO_W,VIDEO_H,GL_RGBA,GL_UNSIGNED_BYTE, (void *)0);
    int64_t startTime = GetSysCurrentTime();
    glBindBuffer(GL_PIXEL_PACK_BUFFER,nextPboId);
    void * mapBuffer = glMapBufferRange(GL_PIXEL_PACK_BUFFER,0,rgb720Length,GL_MAP_READ_BIT);
    int64_t readTime = GetSysCurrentTime() - startTime;
    startTime = GetSysCurrentTime();
    if (mapBuffer == nullptr) return;
    auto * mapCopy720Data = new uint8_t [rgb720Length];
    memset(mapCopy720Data,0x00,rgb720Length);
    memcpy(mapCopy720Data,mapBuffer,rgb720Length);

    auto * openGlImage = new NativeOpenGLImage;
    openGlImage->width = VIDEO_W;
    openGlImage->height = VIDEO_H;
    openGlImage->format = IMAGE_FORMAT_RGBA;
    openGlImage->pLineSize[0] = VIDEO_W * 4;
    openGlImage->ppPlane[0] = mapCopy720Data;
    readPixelCall(3,openGlImage);

    glUnmapBuffer(GL_PIXEL_PACK_BUFFER);
    glBindBuffer(GL_PIXEL_PACK_BUFFER,0);
    int64_t mapTime = GetSysCurrentTime() - startTime;
    LOGCATE("log currentPbo:%d nextPbo:%d asyncReadTime:%lld mapCopyTime:%lld rgbTo420pTime:%lld scaleTime:%lld"
    ,currentPboId,nextPboId,readTime,mapTime,0LL,0LL);
    return;
    // 这里假定测试下，加载
//    LoadImageInfo loadImageInfo;
//    loadImageInfo.loadImage("");
    // 1080rgb -> 1080 yuv420p

    startTime = GetSysCurrentTime();
//    yuvRgbaToI420(mapCopy720Data,yuv420p720,VIDEO_W,VIDEO_H);
    int64_t timeRgbTo420p = GetSysCurrentTime() - startTime;
    startTime = GetSysCurrentTime();
    // 1080 yuv420p -> 720 yuv420p
    int64_t timeScale = GetSysCurrentTime() - startTime;

//    readPixelCall(4,yuv420p720);
//    delete [] yuv420p1080;
//    delete [] yuv420p720;

}






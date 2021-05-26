//
// Created by Admin on 2021/5/13.
//

#include "../../play_header/render/VideoRender.h"
#include "../../play_header/model/shader.h"
#include "../../play_header/utils/CustomGLUtils.h"


void VideoRender::Init(){
    shader = new Shader(readGLSLStrFromFile("videoplay/vetex.glsl").c_str(),
            readGLSLStrFromFile("videoplay/fragment.glsl").c_str());

    GLfloat vetex[] = {
            -1.0f, -1.0f, -1.0f,  0.0f, 0.0f,
            1.0f, -1.0f, -1.0f,  1.0f, 0.0f,
            1.0f,  1.0f, -1.0f,  1.0f, 1.0f,
            1.0f,  1.0f, -1.0f,  1.0f, 1.0f,
            -1.0f,  1.0f, -1.0f,  0.0f, 1.0f,
            -1.0f, -1.0f, -1.0f,  0.0f, 0.0f,
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
//    NativeOpenGLImageUtil::AllocNativeImage(nativeOpenGlImage);
    LoadImageInfo imageInfo;
    imageInfo.loadImage("/storage/emulated/0/ffmpegtest/filterImg/test_box.png");
    glBindTexture(GL_TEXTURE_2D,textures[0]);
    imageInfo.setupNormalSetting();
    imageInfo.uploadImageTex2D();
    glBindTexture(GL_TEXTURE_2D,0);
}

void VideoRender::DrawFrame() {

    glClearColor(1.0,1.0,1.0,1.0);
    glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
    shader->use();
    glBindVertexArray(vaoIds[0]);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D,textures[0]);
    shader->setInt("s_TextureMap",0);
    glDrawArrays(GL_TRIANGLES,0,6);
}

void VideoRender::Destroy() {
    glDeleteBuffers(4,vboIds);
    glDeleteVertexArrays(2,vaoIds);
    glDeleteTextures(4,textures);
//    NativeOpenGLImageUtil::FreeNativeImage(nativeOpenGlImage);
}

void VideoRender::copyImage(AVFrame *data){
    NativeOpenGLImage srcImage;
//    srcImage.format = AV_PIX_FMT_RGBA;
//    srcImage.width = data->width;
//    srcImage.height = data->height;
//    srcImage.ppPlane[0] = data->data[0];
//    std::lock_guard<std::mutex> lockGuard(customMutex);
//    NativeOpenGLImageUtil::CopyNativeImage(&srcImage,nativeOpenGlImage);
}

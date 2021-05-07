//
// Created by 字节流动 on 2020/6/11.
//

#include "VideoGLRender.h"
#include <CustomGLUtils.h>
#include <OpenGLImageDef.h>

std::mutex VideoGLRender::m_Mutex;
VideoGLRender* VideoGLRender::s_Instance = nullptr;

GLfloat verticesCoords[] = {
        -1.0f,  1.0f, 0.0f,  // Position 0
        -1.0f, -1.0f, 0.0f,  // Position 1
        1.0f,  -1.0f, 0.0f,  // Position 2
        1.0f,   1.0f, 0.0f,  // Position 3
};

GLfloat textureCoords[] = {
        0.0f,  0.0f,        // TexCoord 0
        0.0f,  1.0f,        // TexCoord 1
        1.0f,  1.0f,        // TexCoord 2
        1.0f,  0.0f         // TexCoord 3
};

GLushort indices[] = { 0, 1, 2, 0, 2, 3 };


VideoGLRender::~VideoGLRender() {
    NativeOpenGLImageUtil::FreeNativeImage(&m_RenderImage);
    glDeleteTextures(TEXTURE_NUM,m_TextureIds);
    shader->Destroy();
    delete shader;
    shader = 0;
}

VideoGLRender::VideoGLRender() {

}

void VideoGLRender::Init(int videoWidth, int videoHeight, int *dstSize) {
    LOGCATE("VideoGLRender::InitRender video[w, h]=[%d, %d]", videoWidth, videoHeight);
    dstSize[0] = videoWidth;
    dstSize[1] = videoHeight;
    m_FrameIndex = 0;
}

void VideoGLRender::RenderVideoFrame(NativeOpenGLImage *pImage) {
//    LOGCATE("VideoGLRender::RenderVideoFrame pImage=%p", pImage);
    if(pImage == nullptr || pImage->ppPlane[0] == nullptr)
        return;
    std::unique_lock<std::mutex> lock(m_Mutex);
    if(m_RenderImage.ppPlane[0] == nullptr)
    {
        m_RenderImage.format = pImage->format;
        m_RenderImage.width = pImage->width;
        m_RenderImage.height = pImage->height;
        NativeOpenGLImageUtil::AllocNativeImage(&m_RenderImage);
    }

    NativeOpenGLImageUtil::CopyNativeImage(pImage, &m_RenderImage);
}

void VideoGLRender::UnInit() {

}

void VideoGLRender::UpdateMVPMatrix(int angleX, int angleY, float scaleX, float scaleY)
{
    angleX = angleX % 360;
    angleY = angleY % 360;

    //转化为弧度角
    float radiansX = static_cast<float>(MATH_PI / 180.0f * angleX);
    float radiansY = static_cast<float>(MATH_PI / 180.0f * angleY);
    // Projection matrix
    glm::mat4 Projection = glm::ortho(-1.0f, 1.0f, -1.0f, 1.0f, 0.1f, 100.0f);
    //glm::mat4 Projection = glm::frustum(-ratio, ratio, -1.0f, 1.0f, 4.0f, 100.0f);
    //glm::mat4 Projection = glm::perspective(45.0f,ratio, 0.1f,100.f);
    mProjection = Projection;
    // View matrix
    glm::mat4 View = glm::lookAt(
            glm::vec3(0, 0, 4), // Camera is at (0,0,1), in World Space
            glm::vec3(0, 0, 0), // and looks at the origin
            glm::vec3(0, 1, 0)  // Head is up (set to 0,-1,0 to look upside-down)
    );
    mView = View;

    // Model matrix
    glm::mat4 Model = glm::mat4(1.0f);
    Model = glm::scale(Model, glm::vec3(scaleX, scaleY, 1.0f));
    Model = glm::scale(Model, glm::vec3(1.0f));
    Model = glm::rotate(Model, radiansX, glm::vec3(1.0f, 0.0f, 0.0f));
    Model = glm::rotate(Model, radiansY, glm::vec3(0.0f, 1.0f, 0.0f));
//    Model = glm::rotate(Model, 270.0f, glm::vec3(0.0f, 0.0f, 1.0f));
    Model = glm::translate(Model, glm::vec3(0.0f, 0.0f, 0.0f));

    mModel = Model;
    m_MVPMatrix = Projection * View * Model;

}

void VideoGLRender::OnSurfaceCreated() {
    LOGCATE("VideoGLRender::OnSurfaceCreated");
    // 1-正常，2-网格，3-灰色
    int type = 1;
    if ( type == 1) {
        shader = new Shader(readGLSLStrFromFile("opengl_window/vetex.glsl").c_str(),
                            readGLSLStrFromFile("opengl_window/fragment.glsl").c_str());
    } else if (type == 2) {
        shader = new Shader(readGLSLStrFromFile("opengl_window/vetex.glsl").c_str(),
                            readGLSLStrFromFile("opengl_window/meshfrag.glsl").c_str());
    } else {
        shader = new Shader(readGLSLStrFromFile("opengl_window/vetex.glsl").c_str(),
                            readGLSLStrFromFile("opengl_window/grayfrag.glsl").c_str());
    }

    glGenTextures(TEXTURE_NUM, m_TextureIds);
    for (int i = 0; i < TEXTURE_NUM ; ++i) {
        glBindTexture(GL_TEXTURE_2D, m_TextureIds[i]);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glBindTexture(GL_TEXTURE_2D, GL_NONE);
    }

    // Generate VBO Ids and load the VBOs with data
    glGenBuffers(3, m_VboIds);
    glBindBuffer(GL_ARRAY_BUFFER, m_VboIds[0]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(verticesCoords), verticesCoords, GL_STATIC_DRAW);

    glBindBuffer(GL_ARRAY_BUFFER, m_VboIds[1]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(textureCoords), textureCoords, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_VboIds[2]);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    // Generate VAO Id
    glGenVertexArrays(1, &m_VaoId);
    glBindVertexArray(m_VaoId);

    glBindBuffer(GL_ARRAY_BUFFER, m_VboIds[0]);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (const void *)0);
    glBindBuffer(GL_ARRAY_BUFFER, GL_NONE);

    glBindBuffer(GL_ARRAY_BUFFER, m_VboIds[1]);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(GLfloat), (const void *)0);
    glBindBuffer(GL_ARRAY_BUFFER, GL_NONE);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_VboIds[2]);

    glBindVertexArray(GL_NONE);

    m_TouchXY = vec2(0.5f, 0.5f);

    LoadImageInfo imageInfo;
    imageInfo.loadImage("/storage/emulated/0/ffmpegtest/filterImg/test_box.png");
    glBindTexture(GL_TEXTURE_2D,m_TextureIds[3]);
    imageInfo.uploadImageTex2D();
    glBindTexture(GL_TEXTURE_2D,0);


}

void VideoGLRender::OnSurfaceChanged(int w, int h) {
    LOGCATE("VideoGLRender::OnSurfaceChanged [w, h]=[%d, %d]", w, h);
    m_ScreenSize.x = w;
    m_ScreenSize.y = h;
    glViewport(0, 0, w, h);
    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
}

void VideoGLRender::OnDrawFrame() {
    glClear(GL_COLOR_BUFFER_BIT);
    if(shader->ID == GL_NONE|| m_RenderImage.ppPlane[0] == nullptr) return;
    LOGCATE("VideoGLRender::OnDrawFrame [w, h]=[%d, %d], format=%d", m_RenderImage.width, m_RenderImage.height, m_RenderImage.format);
    m_FrameIndex++;

    long long cur = GetSysCurrentTime();
//    if(m_FrameIndex == 2)
//        NativeOpenGLImageUtil::DumpNativeImage(&m_RenderImage, "/sdcard", "2222");

    // upload image data

    LOGCATE("log render format:%d",m_RenderImage.format);
    std::unique_lock<std::mutex> lock(m_Mutex);
    switch (m_RenderImage.format)
    {
        case IMAGE_FORMAT_RGBA:
            glBindTexture(GL_TEXTURE_2D, m_TextureIds[0]);
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, m_RenderImage.width, m_RenderImage.height, 0, GL_RGBA, GL_UNSIGNED_BYTE, m_RenderImage.ppPlane[0]);
            glBindTexture(GL_TEXTURE_2D, GL_NONE);
            break;
        case IMAGE_FORMAT_NV21:
        case IMAGE_FORMAT_NV12:
            //upload Y plane data
            glBindTexture(GL_TEXTURE_2D, m_TextureIds[0]);
            glTexImage2D(GL_TEXTURE_2D, 0, GL_LUMINANCE, m_RenderImage.width,
                         m_RenderImage.height, 0, GL_LUMINANCE, GL_UNSIGNED_BYTE,
                         m_RenderImage.ppPlane[0]);
            glBindTexture(GL_TEXTURE_2D, GL_NONE);

            //update UV plane data
            glBindTexture(GL_TEXTURE_2D, m_TextureIds[1]);
            glTexImage2D(GL_TEXTURE_2D, 0, GL_LUMINANCE_ALPHA, m_RenderImage.width >> 1,
                         m_RenderImage.height >> 1, 0, GL_LUMINANCE_ALPHA, GL_UNSIGNED_BYTE,
                         m_RenderImage.ppPlane[1]);
            glBindTexture(GL_TEXTURE_2D, GL_NONE);
            break;
        case IMAGE_FORMAT_I420:
            //upload Y plane data
            glBindTexture(GL_TEXTURE_2D, m_TextureIds[0]);
            glTexImage2D(GL_TEXTURE_2D, 0, GL_LUMINANCE, m_RenderImage.width,
                         m_RenderImage.height, 0, GL_LUMINANCE, GL_UNSIGNED_BYTE,
                         m_RenderImage.ppPlane[0]);
            glBindTexture(GL_TEXTURE_2D, GL_NONE);

            //update U plane data
            glBindTexture(GL_TEXTURE_2D, m_TextureIds[1]);
            glTexImage2D(GL_TEXTURE_2D, 0, GL_LUMINANCE, m_RenderImage.width >> 1,
                         m_RenderImage.height >> 1, 0, GL_LUMINANCE, GL_UNSIGNED_BYTE,
                         m_RenderImage.ppPlane[1]);
            glBindTexture(GL_TEXTURE_2D, GL_NONE);

            //update V plane data
            glBindTexture(GL_TEXTURE_2D, m_TextureIds[2]);
            glTexImage2D(GL_TEXTURE_2D, 0, GL_LUMINANCE, m_RenderImage.width >> 1,
                         m_RenderImage.height >> 1, 0, GL_LUMINANCE, GL_UNSIGNED_BYTE,
                         m_RenderImage.ppPlane[2]);
            glBindTexture(GL_TEXTURE_2D, GL_NONE);
            break;
        default:
            break;
    }
    lock.unlock();

    UpdateMVPMatrix(0, 0, 1.0f, 1.0f);

    // Use the program object
    shader->use();

    glBindVertexArray(m_VaoId);

    shader -> setMat4( "u_MVPMatrix", m_MVPMatrix);

    for (int i = 0; i < TEXTURE_NUM; ++i) {
        glActiveTexture(GL_TEXTURE0 + i);
        glBindTexture(GL_TEXTURE_2D, m_TextureIds[i]);
        char samplerName[64] = {0};
        sprintf(samplerName, "s_texture%d", i);
        shader -> setInt( samplerName, i);
    }

    //float time = static_cast<float>(fmod(m_FrameIndex, 60) / 50);
    //setFloat(m_ProgramObj, "u_Time", time);

    float offset = (sin(m_FrameIndex * MATH_PI / 25) + 1.0f) / 2.0f;
    shader -> setFloat("u_Offset", offset);
    shader -> setVec2("u_TexSize", vec2(m_RenderImage.width, m_RenderImage.height));
    shader -> setInt("u_nImgType", m_RenderImage.format);

    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, (const void *)0);
    LOGCATE("draw frame spend time:%jd",GetSysCurrentTime() - cur);


    // 画水印
    shader -> setInt("u_nImgType", 1);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, m_TextureIds[3]);
    shader -> setInt("s_texture0", 0);

    mModel = glm::scale(mModel,glm::vec3(0.25f));
    mModel = glm::translate(mModel,glm::vec3(-3.0f,2.0f,0.0f));
    m_MVPMatrix = mProjection * mView * mModel;
    shader -> setMat4( "u_MVPMatrix", m_MVPMatrix);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, (const void *)0);
}

VideoGLRender *VideoGLRender::GetInstance() {
    if(s_Instance == nullptr)
    {
        std::lock_guard<std::mutex> lock(m_Mutex);
        if(s_Instance == nullptr)
        {
            s_Instance = new VideoGLRender;
        }

    }
    return s_Instance;
}

bool VideoGLRender::checkInstanceExist() {
    std::lock_guard<std::mutex> lock(m_Mutex);
    return s_Instance != nullptr;
}

void VideoGLRender::ReleaseInstance() {
    if(s_Instance != nullptr)
    {
        std::lock_guard<std::mutex> lock(m_Mutex);
        if(s_Instance != nullptr)
        {
            delete s_Instance;
            s_Instance = nullptr;
        }

    }
}



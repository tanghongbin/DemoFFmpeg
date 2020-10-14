//
// Created by Admin on 2020/9/28.
//

#include <CustomGLUtils.h>
#include <gtc/matrix_transform.hpp>
#include <TextureSample.h>
#include <OpenGLImageDef.h>
#include "OpenGLFFmpegRender.h"

using namespace std;
OpenGLFFmpegRender *OpenGLFFmpegRender::s_Instance = nullptr;
std::mutex OpenGLFFmpegRender::m_Mutex;
int OpenGLFFmpegRender::mWindowWidth = 0;
int OpenGLFFmpegRender::mWindowHeight = 0;

OpenGLFFmpegRender::OpenGLFFmpegRender() {
    m_TextureId = 0;
}

OpenGLFFmpegRender::~OpenGLFFmpegRender() {
    // 释放缓存图像
    NativeOpenGLImageUtil::FreeNativeImage(&m_RenderImage);
}

void OpenGLFFmpegRender::init(int width, int height) {
    //create RGBA texture
    glGenTextures(1, &m_TextureId);
    glBindTexture(GL_TEXTURE_2D, m_TextureId);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glBindTexture(GL_TEXTURE_2D, GL_NONE);

    char vShaderStr[] =
            "#version 300 es                            \n"
            "layout(location = 0) in vec4 a_position;   \n"
            "layout(location = 1) in vec2 a_texCoord;   \n"
            "out vec2 v_texCoord;                       \n"
            "void main()                                \n"
            "{                                          \n"
            "   gl_Position = a_position;               \n"
            "   v_texCoord = a_texCoord;                \n"
            "}                                          \n";

    char fShaderStr[] =
            "#version 300 es                                     \n"
            "precision mediump float;                            \n"
            "in vec2 v_texCoord;                                 \n"
            "layout(location = 0) out vec4 outColor;             \n"
            "uniform sampler2D s_TextureMap;                     \n"
            "void main()                                         \n"
            "{                                                   \n"
            "  outColor = texture(s_TextureMap, v_texCoord);     \n"
            "}                                                   \n";

    m_ProgramObj = CreateProgram(vShaderStr, fShaderStr, m_VertexShader, m_FragmentShader);
    if (m_ProgramObj)
    {
        m_SamplerLoc = glGetUniformLocation(m_ProgramObj, "s_TextureMap");
    }
    else
    {
        LOGCATE("TextureMapSample::Init create program fail");
    }
}

/**
 * @Deprecated 暂时不用了
 * @param format
 * @param width
 * @param height
 * @param pData
 */
void OpenGLFFmpegRender::SetImageData(int format, int width, int height, uint8_t *pData) {
    LOGCATE("OpenGLFFmpegRender::SetImageData format=%d, width=%d, height=%d, pData=%p", format,
            width, height, pData);
    NativeImage nativeImage;
    nativeImage.format = format;
    nativeImage.width = width;
    nativeImage.height = height;
    nativeImage.ppPlane[0] = pData;

    switch (format) {
        case IMAGE_FORMAT_NV12:
        case IMAGE_FORMAT_NV21:
            nativeImage.ppPlane[1] = nativeImage.ppPlane[0] + width * height;
            break;
        case IMAGE_FORMAT_I420:
            nativeImage.ppPlane[1] = nativeImage.ppPlane[0] + width * height;
            nativeImage.ppPlane[2] = nativeImage.ppPlane[1] + width * height / 4;
            break;
        default:
            break;
    }
//    baseSample->loadImage(&nativeImage);


}

void OpenGLFFmpegRender::unInit() {

}

OpenGLFFmpegRender* OpenGLFFmpegRender::getInstance() {
    if (s_Instance == nullptr){
        std::unique_lock<std::mutex> lock(m_Mutex);
        if (s_Instance == nullptr){
            s_Instance = new OpenGLFFmpegRender;
        }
    }
    return s_Instance;
}

void OpenGLFFmpegRender::destroyInstance() {
    if (s_Instance != nullptr){
        std::unique_lock<std::mutex> lock(m_Mutex);
        if (s_Instance != nullptr){
            delete s_Instance;
            s_Instance = nullptr;
        }
    }
}

void OpenGLFFmpegRender::onSurfaceCreated() {
    glClearColor(1.0f,1.0f,1.0f, 1.0f);
    init(0,0);
}


void OpenGLFFmpegRender::onDrawFrame() {
    LOGCATE("OpenGLFFmpegRender::prepare Draw()");
    if(m_ProgramObj == GL_NONE || m_TextureId == GL_NONE) return;
    LOGCATE("OpenGLFFmpegRender::real Draw()");
    glClear(GL_STENCIL_BUFFER_BIT | GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glClearColor(1.0, 1.0, 1.0, 1.0);

    GLfloat verticesCoords[] = {
            -1.0f,  0.5f, 0.0f,  // Position 0
            -1.0f, -0.5f, 0.0f,  // Position 1
            1.0f, -0.5f, 0.0f,   // Position 2
            1.0f,  0.5f, 0.0f,   // Position 3
    };

    GLfloat textureCoords[] = {
            0.0f,  0.0f,        // TexCoord 0
            0.0f,  1.0f,        // TexCoord 1
            1.0f,  1.0f,        // TexCoord 2
            1.0f,  0.0f         // TexCoord 3
    };

    GLushort indices[] = { 0, 1, 2, 0, 2, 3 };

    //upload RGBA image data
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, m_TextureId);
    unique_lock<mutex> lock(m_Mutex);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, m_RenderImage.width, m_RenderImage.height, 0, GL_RGBA, GL_UNSIGNED_BYTE, m_RenderImage.ppPlane[0]);
    LOGCATE("draw frame width:%d height:%d pImage:%p",m_RenderImage.width,m_RenderImage.height,m_RenderImage.ppPlane[0]);
    lock.unlock();

    glBindTexture(GL_TEXTURE_2D, GL_NONE);

    // Use the program object
    glUseProgram (m_ProgramObj);

    // Load the vertex position
    glVertexAttribPointer (0, 3, GL_FLOAT,
                           GL_FALSE, 3 * sizeof (GLfloat), verticesCoords);
    // Load the texture coordinate
    glVertexAttribPointer (1, 2, GL_FLOAT,
                           GL_FALSE, 2 * sizeof (GLfloat), textureCoords);

    glEnableVertexAttribArray (0);
    glEnableVertexAttribArray (1);

    // Bind the RGBA map
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, m_TextureId);

    // Set the RGBA map sampler to texture unit to 0
    glUniform1i(m_SamplerLoc, 0);

    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, indices);
}

void OpenGLFFmpegRender::onSurfaceChanged(int width, int height) {
    mWindowWidth = width;
    mWindowHeight = height;
    glViewport(0, 0, width, height);
}


void OpenGLFFmpegRender::setupImage(NativeOpenGLImage* image) {
    unique_lock<mutex> lock(m_Mutex);
    if (image == nullptr || image->ppPlane[0] == nullptr){
        LOGCATE("the image will draw are empty");
        return;
    }
    if (m_RenderImage.ppPlane[0] == nullptr){
        m_RenderImage.format = image->format;
        m_RenderImage.width = image->width;
        m_RenderImage.height = image->height;
        NativeOpenGLImageUtil::AllocNativeImage(&m_RenderImage);
    }
    NativeOpenGLImageUtil::CopyNativeImage(image,&m_RenderImage);
    LOGCATE("setup image success");
}


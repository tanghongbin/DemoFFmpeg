//
// Created by Admin on 2021/7/16.
//

#ifndef DEMOFFMPEG_VIDEOCONFIGURATION_H
#define DEMOFFMPEG_VIDEOCONFIGURATION_H

#define DEFAULT_VIDEO_HEIGHT  1280
#define DEFAULT_VIDEO_WIDTH  720
#define DEFAULT_VIDEO_FPS  30
#define DEFAULT_VIDEO_MAX_BPS  3000
#define DEFAULT_VIDEO_MIN_BPS  2000
#define DEFAULT_VIDEO_IFI  10
#define DEFAULT_VIDEO_MIME  "video/avc"
#define DEFAULT_VIDEO_FORMAT 19  //yuv - i420

typedef struct _video_configInfo{
public:
    int width;
    int height;
    int fps;
    int maxBps;
    int minBps;
    int ifi;
    int colorFormat;
    const char * videoMime;
    static struct _video_configInfo* defaultBuild(){
        struct _video_configInfo* info = new struct _video_configInfo;
        info->width = DEFAULT_VIDEO_WIDTH;
        info->height = DEFAULT_VIDEO_HEIGHT;
        info->fps = DEFAULT_VIDEO_FPS;
        info->maxBps = DEFAULT_VIDEO_MAX_BPS;
        info->minBps = DEFAULT_VIDEO_MIN_BPS;
        info->ifi = DEFAULT_VIDEO_IFI;
        info->videoMime = DEFAULT_VIDEO_MIME;
        info->colorFormat = DEFAULT_VIDEO_FORMAT;
        return info;
    }
    static void copy(struct _video_configInfo* dst,struct _video_configInfo* src) {
        dst->width = src->width;
        dst->height = src->height;
        dst->fps = src->fps;
        dst->maxBps = src->maxBps;
        dst->minBps = src->minBps;
        dst->ifi = src->ifi;
        dst->videoMime = src->videoMime;
        dst->colorFormat = src->colorFormat;
    }
    void toString(){
        LOGCATE("打印编码信息 width:%d height:%d fps:%d maxBps:%d ifi:%d mime:%s colorformat:%d",
                width,height,fps,maxBps,ifi,videoMime,colorFormat);
    }
} VideoConfigInfo;



#endif //DEMOFFMPEG_VIDEOCONFIGURATION_H

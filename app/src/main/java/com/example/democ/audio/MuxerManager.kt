package com.example.democ.audio

import android.media.MediaCodec
import android.media.MediaFormat
import android.media.MediaMuxer
import android.os.Environment
import com.example.democ.utils.getRandomStr
import java.io.File
import java.nio.ByteBuffer

class MuxerManager {

    companion object {
        private var instance1: MuxerManager? = null
        @Synchronized
        fun getInstance(): MuxerManager {
            if (instance1 == null) {
                instance1 = MuxerManager()
            }
            return instance1!!
        }
        val FILD_DIR = "/ffmpegtest"
        val MP4_PATH = Environment.getExternalStorageDirectory().absolutePath + "${FILD_DIR}/test.mp4"
        val MP4_PLAY_PATH = Environment.getExternalStorageDirectory().absolutePath + "${FILD_DIR}/video.mp4"
        val MP4_PLAY_BIG_PATH = Environment.getExternalStorageDirectory().absolutePath + "${FILD_DIR}/blackanimal.mp4"
        val SLIGHT_MP4_VIDEO_PATH = Environment.getExternalStorageDirectory().absolutePath + "${FILD_DIR}/test5.mp4"
        val YUV_PATH = Environment.getExternalStorageDirectory().absolutePath + "${FILD_DIR}/YUV_Image_840x1074.NV21"
        val ENCODE_VIDEO_PATH = Environment.getExternalStorageDirectory().absolutePath + "${FILD_DIR}/encodeVideo.h264"

        val FILTER_ADD_TO_YUV_PATH = Environment.getExternalStorageDirectory().absolutePath + "${FILD_DIR}/filterAddToYuv.h264"
        val SWS_CONVERT_PNG = Environment.getExternalStorageDirectory().absolutePath + "/sws_after_scale.png"


        // =================== download -=====================================
        val MP4_DOWNLOAD_FUNNY_VIDEO = Environment.getExternalStorageDirectory().absolutePath + "${FILD_DIR}/funny_video.mp4"
        val FLV_DOWNLOAD_MY_LOGO = Environment.getExternalStorageDirectory().absolutePath + "${FILD_DIR}/my_logo.flv"
    }

    private lateinit var muxer: MediaMuxer

    fun init() {
        // 确保这个地址是一个新文件
//        val file = File(MP4_PATH)
//        if (file.exists()){
//            file.delete()
//        }
//        val result = file.createNewFile()
//        log("文件创建结果:${result}")
        val path = getRandomStr("hwencodemp4/",".mp4")
        muxer = MediaMuxer(path, MediaMuxer.OutputFormat.MUXER_OUTPUT_MPEG_4)
//        log("初始化成功")
    }

    private var count = 0
    private var isStarted = false



    fun writeSampleData(trackIndex:Int,byteBuffer: ByteBuffer,bufferInfo: MediaCodec.BufferInfo){
//        log("i'm writeSampleData processing")
        muxer.writeSampleData(trackIndex,byteBuffer,bufferInfo)
    }

    @Synchronized
    fun addTrack(mediaFormat: MediaFormat): Int {
        count++
        log("添加format:${mediaFormat}")
        return muxer.addTrack(mediaFormat)
    }

    @Synchronized
    fun start() {
        if (isStarted || count < 2) return
        muxer.start()
        isStarted = true
        log("开始混合了")
    }

    fun stop() {
        isStarted = false
        count = 0
        muxer.stop()
        muxer.release()
        instance1 = null
    }

    fun isReady():Boolean{
//        log("准备结果:${count == 2}")
        return count == 2
    }

}
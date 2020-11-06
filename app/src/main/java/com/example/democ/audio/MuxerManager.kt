package com.example.democ.audio

import android.media.MediaCodec
import android.media.MediaFormat
import android.media.MediaMuxer
import android.os.Environment
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
        val H264_PLAY_PATH = Environment.getExternalStorageDirectory().absolutePath + "${FILD_DIR}/video.mp4"
        val MP4_PLAY_BIG_PATH = Environment.getExternalStorageDirectory().absolutePath + "${FILD_DIR}/blackanimal.mp4"
        val MP4_PLAY_BIG_TEST_5_PATH = Environment.getExternalStorageDirectory().absolutePath + "${FILD_DIR}test5.mp4"
        val YUV_PATH = Environment.getExternalStorageDirectory().absolutePath + "${FILD_DIR}/YUV_Image_840x1074.NV21"
        val ENCODE_VIDEO_PATH = Environment.getExternalStorageDirectory().absolutePath + "${FILD_DIR}/encodeVideo.h264"

        val SWS_CONVERT_PNG = Environment.getExternalStorageDirectory().absolutePath + "/sws_after_scale.png"
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
        muxer = MediaMuxer(MP4_PATH, MediaMuxer.OutputFormat.MUXER_OUTPUT_MPEG_4)
//        log("初始化成功")
    }

    private var count = 0
    private var isStarted = false



    fun writeSampleData(trackIndex:Int,byteBuffer: ByteBuffer,bufferInfo: MediaCodec.BufferInfo){
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
        muxer.stop()
        muxer.release()
        instance1 = null
    }

    fun isReady():Boolean{
//        log("准备结果:${count == 2}")
        return count == 2
    }

}
package com.example.democ.audio

import android.media.MediaCodec
import android.media.MediaFormat
import android.media.MediaMuxer
import com.example.common_base.utils.getRandomStr
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
        val path =
            getRandomStr("hwencodemp4/", ".mp4")
        muxer = MediaMuxer(path, MediaMuxer.OutputFormat.MUXER_OUTPUT_MPEG_4)
//        log("初始化成功")
    }

    private var count = 0
    private var isStarted = false



    @Synchronized
    fun writeSampleData(trackIndex:Int,byteBuffer: ByteBuffer,bufferInfo: MediaCodec.BufferInfo){
//        log("i'm writeSampleData processing")
        if (!isStarted) {
            return
        }
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

    @Synchronized
    fun stop() {
        isStarted = false
        muxer.stop()
        muxer.release()
        instance1 = null
    }

    @Synchronized
    fun isReady():Boolean{
//        log("准备结果:${count == 2}")
        return count == 2
    }

}
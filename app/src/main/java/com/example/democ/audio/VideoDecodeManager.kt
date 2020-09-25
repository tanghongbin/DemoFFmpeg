package com.example.democ.audio

import android.media.MediaCodec
import android.media.MediaExtractor
import android.media.MediaFormat
import android.os.Build
import android.view.Surface
import androidx.annotation.RequiresApi

class VideoDecodeManager {

    private var mediaExtractor: MediaExtractor? = null
    private var mediaFormat: MediaFormat? = null
    private var mediaCodec: MediaCodec? = null
    private var isClosed = false
    private val mSpeedController: SpeedManager = SpeedManager()

    fun init(surface: Surface) {
        mediaCodec = MediaCodec.createDecoderByType("video/avc")
        mediaFormat = MediaFormat.createVideoFormat("video/avc", 1280, 720)
        mediaExtractor = MediaExtractor()
        //MP4 文件存放位置
        //MP4 文件存放位置
        mediaExtractor?.setDataSource(MuxerManager.MP4_PLAY_PATH)
        for (index in 0 until mediaExtractor!!.trackCount) {
            val trackFormat = mediaExtractor?.getTrackFormat(index)
            val mime: String = trackFormat!!.getString(MediaFormat.KEY_MIME)
            if (mime.startsWith("video")) {
                mediaFormat = trackFormat
                mediaExtractor!!.selectTrack(index)
            }
        }
        log("getTrackCount: " + mediaExtractor?.getTrackCount())
        mediaCodec?.configure(mediaFormat, surface, null, 0)
        mediaCodec?.start()
    }




    fun startDecode(){
        // 启动线程解码视频
        buildTask().start()
    }

    fun buildTask() = object : Thread() {
        @RequiresApi(Build.VERSION_CODES.LOLLIPOP)
        override fun run() {
            while (!isClosed){
                val index = mediaCodec!!.dequeueInputBuffer(-1)
                log("prepare decode video index:${index}")
                if (index >= 0){
                    val buffer = mediaCodec?.getInputBuffer(index)
                    buffer?.apply {
                        val samSize = mediaExtractor!!.readSampleData(this,0)
                        val samTime = mediaExtractor!!.sampleTime
                        if (samSize > 0 && samTime > 0){
                            mediaCodec?.queueInputBuffer(index,0,samSize,samTime,0)
                            mSpeedController.preRender(samTime)
                            mediaExtractor?.advance()
                        }
                    }
                }
                log("读取视频数据:${index}")
                val info = MediaCodec.BufferInfo()
                val outIndex = mediaCodec!!.dequeueOutputBuffer(info,0)
                if (outIndex >= 0) {
                    mediaCodec?.releaseOutputBuffer(outIndex, true)
                }
            }
            log("loop task has finished")
        }
    }

    fun stopDecode(){
        // 关闭线程，释放解码器
        isClosed = true
        mediaCodec?.stop()
        mediaCodec?.release()
        mediaCodec = null
    }



}
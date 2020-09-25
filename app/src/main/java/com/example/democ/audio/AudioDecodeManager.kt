package com.example.democ.audio

import android.media.*
import android.os.Build
import androidx.annotation.RequiresApi

class AudioDecodeManager {

    private var mediaExtractor: MediaExtractor? = null
    private var mediaFormat: MediaFormat? = null
    private var mediaCodec: MediaCodec? = null
    private var isClosed = false
    private val mAudioPlayer = AudioPlayer()

    private val DEFAULT_MIME_TYPE = "audio/mp4a-latm"
    private val DEFAULT_CHANNEL_NUM = 1
    private val DEFAULT_SAMPLE_RATE = 44100
    private val DEFAULT_MAX_BUFFER_SIZE = 16384

    fun init() {
        mediaCodec = MediaCodec.createDecoderByType("audio/mp4a-latm")
        mediaExtractor = MediaExtractor()
        //MP4 文件存放位置
        //MP4 文件存放位置
        mediaExtractor?.setDataSource(MuxerManager.MP4_PLAY_PATH)
        for (index in 0 until mediaExtractor!!.trackCount) {
            val trackFormat = mediaExtractor?.getTrackFormat(index)
            val mime: String = trackFormat!!.getString(MediaFormat.KEY_MIME)
            if (mime.startsWith("audio")) {
                mediaFormat = trackFormat
                mediaExtractor!!.selectTrack(index)
            }
        }
        log("getTrackCount: " + mediaExtractor?.getTrackCount())
        mediaCodec?.configure(mediaFormat, null, null, 0)
        mediaCodec?.start()
        mAudioPlayer.startPlayer()
    }


    fun startDecode() {
        // 启动线程解码视频
        buildTask().start()
    }

    fun buildTask() = object : Thread() {
        @RequiresApi(Build.VERSION_CODES.LOLLIPOP)
        override fun run() {
            while (!isClosed) {
                val index = mediaCodec!!.dequeueInputBuffer(-1)
                log("prepare decode audio index:${index}")
                if (index >= 0) {
                    val buffer = mediaCodec?.getInputBuffer(index)
                    buffer?.apply {
                        val samSize = mediaExtractor!!.readSampleData(this, 0)
                        val samTime = mediaExtractor!!.sampleTime
                        if (samSize > 0 && samTime > 0) {
                            mediaCodec?.queueInputBuffer(index, 0, samSize, samTime, 0)
                            mediaExtractor?.advance()
                        }
                    }
                }
                log("读取音数据:${index}")
                val info = MediaCodec.BufferInfo()
                val outIndex = mediaCodec!!.dequeueOutputBuffer(info, 0)
                if (outIndex >= 0) {
                    val byteArray = ByteArray(info.size)
                    mediaCodec?.getOutputBuffer(outIndex)?.get(byteArray)
                    mAudioPlayer.play(byteArray, 0, info.size)
                    mediaCodec?.releaseOutputBuffer(outIndex, true)
                }
            }
            log("loop task has finished")
        }
    }

    fun stopDecode() {
        // 关闭线程，释放解码器
        isClosed = true
        mediaCodec?.stop()
        mediaCodec?.release()
        mediaCodec = null
    }


}
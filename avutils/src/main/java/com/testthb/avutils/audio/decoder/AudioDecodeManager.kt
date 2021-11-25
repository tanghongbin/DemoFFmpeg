package com.testthb.avutils.audio.decoder

import android.media.*
import android.os.Build
import androidx.annotation.RequiresApi
import com.testthb.common_base.utils.Constants
import com.testthb.common_base.utils.log
import java.lang.Exception
import java.util.concurrent.atomic.AtomicLong

class AudioDecodeManager {

    private var mediaExtractor: MediaExtractor? = null
    private var mediaFormat: MediaFormat? = null
    private var mediaCodec: MediaCodec? = null
    private var isClosed = false
    private val mAudioPlayer = AudioPlayer()
    private var mAudioDecodeThread:Thread? = null
    private var mSeekProgress = AtomicLong(-1L)

    private val DEFAULT_MIME_TYPE = "audio/mp4a-latm"
    private val DEFAULT_CHANNEL_NUM = 1
    private val DEFAULT_SAMPLE_RATE = 44100
    private val DEFAULT_MAX_BUFFER_SIZE = 16384

    companion object{
        private var instance: AudioDecodeManager? = null
        fun getInstance(): AudioDecodeManager {
            if (instance == null){
                synchronized(AudioDecodeManager::class.java){
                    if (instance == null){
                        instance = AudioDecodeManager()
                    }
                }
            }
            return instance!!
        }
        fun destroyInstance(){
            instance = null
        }
    }

    fun seekTo(progress:Int){
        mSeekProgress.compareAndSet(-1,progress.toLong())
    }

    fun init(audioPath:String) {
        mediaCodec = MediaCodec.createDecoderByType("audio/mp4a-latm")
        mediaExtractor = MediaExtractor()
        //MP4 文件存放位置
        mediaExtractor?.setDataSource(audioPath)
        for (index in 0 until mediaExtractor!!.trackCount) {
            val trackFormat = mediaExtractor?.getTrackFormat(index)
            val mime: String = trackFormat!!.getString(MediaFormat.KEY_MIME) ?: ""
            if (mime.startsWith("audio")) {
                log("has looked up mediaformat")
                mediaFormat = trackFormat
                mediaExtractor!!.selectTrack(index)
            }
        }
        log("getTrackCount: " + mediaExtractor?.trackCount)
        try {
            mediaCodec?.configure(mediaFormat, null, null, 0)
            mediaCodec?.start()
            mAudioPlayer.startPlayer()
            startDecode()
        }catch (e:IllegalArgumentException){
            mAudioPlayer.stopPlayer()
        }

    }


    private fun startDecode() {
        // 启动线程解码视频
        mAudioDecodeThread = buildTask()
        mAudioDecodeThread?.start()
    }

    private fun buildTask() = object : Thread() {
        @RequiresApi(Build.VERSION_CODES.LOLLIPOP)
        override fun run() {
            while (!isClosed) {
                val index = mediaCodec!!.dequeueInputBuffer(1000)
//                log("prepare decode audio index:${index}")
                if (index >= 0) {
                    val buffer = mediaCodec?.getInputBuffer(index)
                    buffer?.apply {
                        val samSize = mediaExtractor!!.readSampleData(this, 0)
                        val samTime = mediaExtractor!!.sampleTime
                        if (samSize > 0 && samTime > -1) {
                            mediaCodec?.queueInputBuffer(index, 0, samSize, samTime, 0)
                        } else {
                            log("audio 已经退出")
                            return
                        }
                    }
                }
                val info = MediaCodec.BufferInfo()
                val outIndex = mediaCodec!!.dequeueOutputBuffer(info, 1000)
//                log("输出音数据帧:${outIndex}")
                if (outIndex >= 0) {
                    val byteArray = ByteArray(info.size)
                    mediaCodec?.getOutputBuffer(outIndex)?.get(byteArray)
                    mAudioPlayer.play(byteArray, 0, info.size)
                    mediaCodec?.releaseOutputBuffer(outIndex, true)
                }

                if (mSeekProgress.get() != -1L){
                    mediaExtractor?.seekTo(mSeekProgress.get() * Constants.TIME_UNIT_SEC,
                        MediaExtractor.SEEK_TO_PREVIOUS_SYNC)
                    log("上设置成功，当前progress:${mSeekProgress.get()}")
                    mSeekProgress.set(-1)
                    mediaCodec?.flush()
                }
                mediaExtractor?.advance()
            }
            log("loop task has finished")
        }
    }

    fun stopDecode() {
        // 关闭线程，释放解码器
        try {
            if (isClosed) return
            isClosed = true
            mAudioDecodeThread?.join(1500)
            mAudioPlayer.stopPlayer()
            mediaCodec?.stop()
            mediaCodec?.release()
            mediaExtractor?.release()
            mediaCodec = null
        }catch (e:Exception){
            e.printStackTrace()
        }
    }


}
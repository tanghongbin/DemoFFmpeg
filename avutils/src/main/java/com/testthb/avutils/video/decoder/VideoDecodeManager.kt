@file:Suppress("ConvertSecondaryConstructorToPrimary")

package com.testthb.avutils.video.decoder

import android.media.*
import android.os.Build
import android.view.Surface
import androidx.annotation.RequiresApi
import com.testthb.common_base.utils.log
import java.util.concurrent.TimeUnit
import java.util.concurrent.atomic.AtomicLong

class VideoDecodeManager {

    private var mediaExtractor: MediaExtractor? = null
    private var mediaFormat: MediaFormat? = null
    private var mediaCodec: MediaCodec? = null
    private var isClosed = false
    private var mOutputFormatChangeListener:OutputFormatChangeListener? = null
    private val mSpeedController: SpeedManager = SpeedManager()
    private var mVideoDecoderThread:Thread? = null
    private var mSeekProgress = AtomicLong(-1L)


    fun setOutputFormatChanged(listener: OutputFormatChangeListener){
        mOutputFormatChangeListener = listener
    }

    private constructor()

    companion object{
        private var instance:VideoDecodeManager? = null
        fun getInstance():VideoDecodeManager{
            if (instance == null){
                synchronized(VideoDecodeManager::class.java){
                    if (instance == null){
                        instance = VideoDecodeManager()
                    }
                }
            }
            return instance!!
        }
        fun destroyInstance(){
            instance = null
        }
    }

    fun init(surface: Surface,videoPath:String) {
//        mediaFormat!!.setInteger(MediaFormat.KEY_COLOR_FORMAT, MediaCodecInfo.CodecCapabilities.COLOR_FormatYUV420SemiPlanar);
        mediaExtractor = MediaExtractor()
        //MP4 文件存放位置
        mediaExtractor?.setDataSource(videoPath)
        for (index in 0 until mediaExtractor!!.trackCount) {
            val trackFormat = mediaExtractor?.getTrackFormat(index)
            val mime: String = trackFormat!!.getString(MediaFormat.KEY_MIME) ?: ""
            if (mime.startsWith("video")) {
                mediaFormat = trackFormat
                mediaCodec = MediaCodec.createDecoderByType(mediaFormat?.getString(MediaFormat.KEY_MIME) ?: "")
                mediaExtractor!!.selectTrack(index)
            }
        }
        try {
            mediaCodec?.configure(mediaFormat, surface, null, 0)
            mediaCodec?.start()
            startDecode()
        }catch (e:IllegalArgumentException){
            e.printStackTrace()
        }
    }

    private fun startDecode(){
        // 启动线程解码视频
        mVideoDecoderThread = buildTask()
        mVideoDecoderThread?.start()
    }

    fun seekTo(progress:Int){
        mSeekProgress.compareAndSet(-1,progress.toLong())
    }

    private fun buildTask() = object : Thread() {
        @RequiresApi(Build.VERSION_CODES.LOLLIPOP)
        override fun run() {
            while (!isClosed){
                val index = mediaCodec!!.dequeueInputBuffer(0)
//                log("prepare decode video index:${index}")
                if (index >= 0){
                    val buffer = mediaCodec?.getInputBuffer(index)
                    buffer?.apply {
                        val samSize = mediaExtractor!!.readSampleData(this,0)
                        val samTime = mediaExtractor!!.sampleTime
                        val result = mediaExtractor?.advance()
                        if (samSize > 0 && samTime > -1){
                            mOutputFormatChangeListener?.currentDuration(samTime)
                            mediaCodec?.queueInputBuffer(index,0,samSize,samTime,0)
                            mSpeedController.preRender(samTime)
                            if (mSeekProgress.get() != -1L){
                                val seekResult = TimeUnit.SECONDS.toMicros(mSeekProgress.get())
                                mediaExtractor?.seekTo(seekResult,
                                        MediaExtractor.SEEK_TO_PREVIOUS_SYNC)
                                log("上设置成功，当前progress:${mSeekProgress.get()} seekResult:${seekResult}")
                                mSeekProgress.set(-1L)
                                log("after time:${mediaExtractor?.sampleTime}")
                            }
                        } else {
                            log("已经退出 size:${samSize} time:${samTime}")
                            return
                        }
                    }
                }
                log("读取视频数据:${index}")
                val info = MediaCodec.BufferInfo()
                val outIndex = mediaCodec!!.dequeueOutputBuffer(info,0)
                if (outIndex == MediaCodec.INFO_OUTPUT_FORMAT_CHANGED){
                    val width = mediaFormat?.getInteger(MediaFormat.KEY_WIDTH)
                    val height = mediaFormat?.getInteger(MediaFormat.KEY_HEIGHT)
                    val duration = mediaFormat?.getLong(MediaFormat.KEY_DURATION)
                    mOutputFormatChangeListener?.outputFormatChange(width,height,duration)
                }
                if (outIndex >= 0) {
                    mediaCodec?.releaseOutputBuffer(outIndex, true)
                }
            }
            log("loop task has finished")
        }
    }

    /*
    *
    * 查找这个时间点对应的最接近的一帧。
    * 这一帧的时间点如果和目标时间相差不到 一帧间隔 就算相近
    *
    * maxRange:查找范围
    * */
//    fun getValidSampleTime(time: Long, maxRange: Int = 5): Long {
//        if (mediaExtractor == null) return 0L
//        mediaExtractor!!.seekTo(time, MediaExtractor.SEEK_TO_PREVIOUS_SYNC)
//        var count = 0
//        var sampleTime = mediaExtractor!!.sampleTime
//        while (count < maxRange) {
//            mediaExtractor!!.advance()
//            val s = mediaExtractor!!.sampleTime
//            if (s != -1L) {
//                count++
//                // 选取和目标时间差值最小的那个
//                sampleTime = time.minDifferenceValue(sampleTime, s)
//                if (abs(sampleTime - time) <= perFrameTime) {
//                    //如果这个差值在 一帧间隔 内，即为成功
//                    return sampleTime
//                }
//            } else {
//                count = maxRange
//            }
//        }
//        return sampleTime
//    }


    fun stopDecode(){
        // 关闭线程，释放解码器
        try {
            if (isClosed) return
            isClosed = true
            mVideoDecoderThread?.join(1500)
            mediaCodec?.stop()
            mediaCodec?.release()
            mediaCodec = null
        }catch (e:Exception){
            e.printStackTrace()
        }
    }



}

interface OutputFormatChangeListener{
    fun outputFormatChange(width: Int?, height: Int?, duration: Long?)
    fun currentDuration(time:Long)
}
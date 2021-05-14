package com.example.avutils.video.decoder

import android.media.MediaCodec
import android.media.MediaExtractor
import android.media.MediaFormat
import android.os.Build
import android.view.Surface
import androidx.annotation.RequiresApi
import com.example.common_base.utils.Constants
import com.example.common_base.utils.log
import java.util.concurrent.atomic.AtomicInteger
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
        mediaCodec = MediaCodec.createDecoderByType("video/avc")
        mediaFormat = MediaFormat.createVideoFormat("video/avc", 1280, 720)
//        mediaFormat!!.setInteger(MediaFormat.KEY_COLOR_FORMAT, MediaCodecInfo.CodecCapabilities.COLOR_FormatYUV420SemiPlanar);
        mediaExtractor = MediaExtractor()
        //MP4 文件存放位置
        mediaExtractor?.setDataSource(videoPath)
        for (index in 0 until mediaExtractor!!.trackCount) {
            val trackFormat = mediaExtractor?.getTrackFormat(index)
            val mime: String = trackFormat!!.getString(MediaFormat.KEY_MIME)
            if (mime.startsWith("video")) {
                mediaFormat = trackFormat
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
                val index = mediaCodec!!.dequeueInputBuffer(1000)
//                log("prepare decode video index:${index}")
                if (index >= 0){
                    val buffer = mediaCodec?.getInputBuffer(index)
                    buffer?.apply {
                        val samSize = mediaExtractor!!.readSampleData(this,0)
                        val samTime = mediaExtractor!!.sampleTime
                        if (samSize > 0 && samTime > -1){
                            mOutputFormatChangeListener?.currentDuration(samTime)
                            mediaCodec?.queueInputBuffer(index,0,samSize,samTime,0)
                            mSpeedController.preRender(samTime)
                        } else {
                            log("已经退出")
                            return
                        }
                    }
                }
//                log("读取视频数据:${index}")
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
                if (mSeekProgress.get() != -1L){
                    mediaExtractor?.seekTo(mSeekProgress.get() * Constants.TIME_UNIT_SEC,
                        MediaExtractor.SEEK_TO_CLOSEST_SYNC)
                    log("上设置成功，当前progress:${mSeekProgress.get()}")
                    mSeekProgress.set(-1)
                    mediaCodec?.flush()
                }
                mediaExtractor?.advance()
            }
            log("loop task has finished")
        }
    }

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
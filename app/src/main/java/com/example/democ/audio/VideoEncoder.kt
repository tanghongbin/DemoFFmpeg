package com.example.democ.audio

import android.animation.TimeAnimator
import android.media.MediaCodec
import android.media.MediaCodecInfo
import android.media.MediaFormat
import android.os.Build
import androidx.annotation.RequiresApi
import com.example.democ.MainActivity.Companion.log
import com.example.democ.hwencoder.VideoConfiguration
import com.example.democ.hwencoder.VideoMediaCodec
import com.example.democ.interfaces.OutputEncodedDataListener
import com.example.democ.utils.JavaYuvConvertHelper
import com.example.democ.utils.TimeTracker
import com.libyuv.LibyuvUtil
//import com.libyuv.util.YuvUtil
import java.io.IOException
import java.nio.ByteBuffer
import java.util.concurrent.LinkedBlockingDeque

/**
 *
 * author : tanghongbin
 *
 * date   : 2020/7/27 14:27
 *
 * desc   : 编码器,通过音频和视频的原始数据编码成MP4
 * 文件
 *
 **/
class VideoEncoder {


    companion object {
        private final const val MAX_SIZE = 100
    }

    private lateinit var mediaCodec: MediaCodec
    private lateinit var yuv420spsrc: ByteArray
    private var mWidth = 0
    private var mHeight = 0
    private var isClosed = false
    private var hasSetOretation = false
    private var mDegree: Int = 0
    private val videoByteList = LinkedBlockingDeque<ByteArray?>()
    var trackId: Int = 0
    var thread: Thread? = null
    var startTime = 0L
    private var mOutputListener: OutputEncodedDataListener? = null

    fun setOutputListener(listener: OutputEncodedDataListener?) {
        this.mOutputListener = listener
    }

    fun setDimensions(width: Int, height: Int) {
        mWidth = width
        mHeight = height
    }

    fun setDegrees(degree: Int) {
        hasSetOretation = true
        mDegree = degree
    }


    private fun initMediaCodec() {
        log("width:${mWidth} --mHeight:${mHeight} 分配内存大小:${mWidth * mHeight * 3 / 2}")
        //编码格式，AVC对应的是H264

        val config = VideoConfiguration.Builder().apply {
            setSize(mHeight, mWidth)
                .setBps(1200, 1800)
                .setFps(20)
                .setIfi(1)
        }.build()
        mediaCodec = VideoMediaCodec.getVideoMediaCodec(config)
        yuv420spsrc = ByteArray(mHeight * mWidth * 3 / 2)
        //进行生命周期执行状态
        mediaCodec.start()
    }

    fun startEncode() {
        initMediaCodec()
        // 开启线程轮询编码
        thread = innerThread()
        thread?.start()
        startTime = System.nanoTime()
//        log("视频的开始时间:${}")
    }

    fun stopEncode() {
        isClosed = true
        log("当前列表剩余:${videoByteList.size}")
//        videoByteList.clear()
        try {
            thread?.interrupt()
            thread?.join(1000)
        } catch (e: InterruptedException) {
            log("InterruptedException $e")
        }
        log(
            " EncodeThread isAlive: " + thread?.isAlive
        )
        if (mediaCodec != null) {
            mediaCodec.stop()
            mediaCodec.release()
        }
        log("video encode thread has finished")
    }

    private fun innerThread(): Thread {
        return object : Thread() {

            @RequiresApi(Build.VERSION_CODES.LOLLIPOP)
            override fun run() {

                while (!isClosed) {
//                    log("has encode video start")
                    val innerStart = System.currentTimeMillis()
                    try {
                        val inputBuff = getInputBuffer()
                        @Suppress("FoldInitializerAndIfToElvis")
                        if (inputBuff == null) {
                            log("get input buffer is null,must finished")
                            break
                        }
                        val bufferId = mediaCodec.dequeueInputBuffer(1000)
                        if (bufferId > 0) {
                            val byteBuffer = mediaCodec.getInputBuffer(bufferId)
                            byteBuffer?.put(inputBuff)
                            mediaCodec.queueInputBuffer(
                                bufferId,
                                0,
                                inputBuff.size,
                                (System.nanoTime() - startTime) / 1000,
                                0
                            )
                        }
//                    log("has encode video middle")
                        val info = MediaCodec.BufferInfo()
                        val outputId = mediaCodec.dequeueOutputBuffer(info, 1000)
                        if (outputId > 0) {
                            val byteBuffer = mediaCodec.getOutputBuffer(outputId)
//                            val byteArray = ByteArray(info.size)
//                            byteBuffer?.get(byteArray)
//                            byteBuffer?.position(info.offset)
//                            byteBuffer?.limit(info.offset + info.size)
                            // 写入混合流中
//                            log("硬编码总时长:${System.currentTimeMillis() - innerStart}")
                            if (byteBuffer != null) {
                                mOutputListener?.outputData(byteBuffer, info)
//                                MuxerManager.getInstance().writeSampleData(trackId, byteBuffer, info)
//                            log("视频写入数据2:$trackId   buffer:$byteBuffer")
                            }
                            //释放output buffer
                            mediaCodec.releaseOutputBuffer(outputId, false)
                        } else if (outputId == MediaCodec.INFO_OUTPUT_FORMAT_CHANGED) {
                            mOutputListener?.outputFormatChanged(mediaCodec.outputFormat)
//                            trackId = MuxerManager.getInstance().addTrack(mediaCodec.outputFormat)
//                            MuxerManager.getInstance().start()
                        }
                    } catch (e: InterruptedException) {
                        e.printStackTrace()
                    }
//                    log("has encode video frame over")

                }
                log("encode video finished")
            }
        }
    }

    fun saveVideoByte(byteArray: ByteArray?) {
//        log("put every frame data:${byteArray?.size}")
        if (videoByteList.size > MAX_SIZE) {
            videoByteList.poll()
        }
        videoByteList.put(byteArray)
    }


    private fun getInputBuffer(): ByteArray? {
        val input = videoByteList.take() ?: return null
        //        val byteArray = ByteArray(mWidth * mHeight * 3 / 2)
//        NV21ToNV12(input, yuv420spsrc, mWidth, mHeight)
        nv212nv12(input, yuv420spsrc, mWidth, mHeight)
        return yuv420spsrc
    }

    private fun nv212nv12(srcData: ByteArray?, dstData: ByteArray, mWidth: Int, mHeight: Int): Int {
        if (srcData == null || dstData == null) {
            return -1
        }
        val windowWidth = mWidth
        val windowHeight = mHeight
        val scaleHeight = mWidth
        val scaleWidth = mHeight
        val mOrientation = mDegree
        if (!hasSetOretation) throw IllegalStateException("mDegree is invalid")
//        log("srcData:${srcData?.size} dstData:${dstData?.size}")

        JavaYuvConvertHelper.convertNv21ToNv12(
            srcData,
            dstData,
            windowWidth,
            windowHeight,
            mOrientation,
            mOrientation == 270
        )
        return 0
    }

}
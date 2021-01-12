package com.example.democ.audio

//import com.libyuv.util.YuvUtil
import android.media.MediaCodec
import android.os.Build
import android.view.Surface
import androidx.annotation.RequiresApi
import com.example.democ.MainActivity.Companion.log
import com.example.democ.hwencoder.HwEncoderHelper
import com.example.democ.hwencoder.VideoConfiguration
import com.example.democ.hwencoder.VideoMediaCodec
import com.example.democ.interfaces.OutputEncodedDataListener
import com.example.democ.interfaces.SingleGetDataInterface
import com.example.democ.interfaces.SingleInterface
import com.example.democ.utils.JavaYuvConvertHelper
import com.example.democ.utils.LogUtils
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
    //    private lateinit var yuv420spsrc: ByteArray
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

    private var mCaptureMode = HwEncoderHelper.CaptureMode.RECORD

    fun setCaptureMode(mode: HwEncoderHelper.CaptureMode) {
        mCaptureMode = mode
    }


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
                .setBps(400, 600)
                .setFps(20)
                .setIfi(1)
        }.build()
        mediaCodec = VideoMediaCodec.getVideoMediaCodec(config)
//        mediaCodec.setInputSurface()
//        yuv420spsrc = ByteArray(mHeight * mWidth * 3 / 2)
        //进行生命周期执行状态
        mediaCodec.start()
    }

    fun startEncode() {
        initMediaCodec()
        // 开启线程轮询编码
        thread = innerThread()
        thread?.start()
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
                startTime = System.nanoTime()
                while (!isClosed) {
                    try {
                        val bufferId = mediaCodec.dequeueInputBuffer(12000)
                        if (bufferId >= 0) {
                            val byteBuffer = mediaCodec.getInputBuffer(bufferId)
                            val inputBuff = getInputBuffer()
                            @Suppress("FoldInitializerAndIfToElvis")
                            if (inputBuff == null) {
                                log("get input buffer is null,must finished")
                                break
                            }
                            byteBuffer?.put(inputBuff)
                            val timeStamp: Long =
                                if (mCaptureMode === HwEncoderHelper.CaptureMode.RECORD) (System.nanoTime() - startTime) / 1000 else 0
                            mediaCodec.queueInputBuffer(
                                bufferId,
                                0,
                                inputBuff.size,
                                timeStamp,
                                0
                            )
                        }
                        val info = MediaCodec.BufferInfo()
                        val outputId = mediaCodec.dequeueOutputBuffer(info, 12000)
                        if (outputId >= 0) {
                            resolveBuffer(outputId, info)
                        } else if (outputId == MediaCodec.INFO_OUTPUT_FORMAT_CHANGED) {
                            mOutputListener?.outputFormatChanged(mediaCodec.outputFormat)
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

    private fun resolveBuffer(outputId: Int, info: MediaCodec.BufferInfo) {
        val byteBuffer = mediaCodec.getOutputBuffer(outputId)
        if (byteBuffer != null) {
            mOutputListener?.outputData(byteBuffer, info)
        }
        mediaCodec.releaseOutputBuffer(outputId, false)
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
        val byteArray = ByteArray(mWidth * mHeight * 3 / 2)
        nv212nv12(input, byteArray, mWidth, mHeight)
        return byteArray
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
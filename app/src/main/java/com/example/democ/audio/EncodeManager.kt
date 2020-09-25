package com.example.democ.audio

import android.media.MediaCodec
import android.media.MediaCodecInfo
import android.media.MediaFormat
import android.os.Build
import androidx.annotation.RequiresApi
import com.example.democ.CustomSurfaceView
import com.example.democ.MainActivity.Companion.log
import java.io.IOException
import java.util.*

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
object EncodeManager {


    private lateinit var mediaCodec: MediaCodec
    private lateinit var yuv420spsrc: ByteArray
    private var mWidth = 0
    private var mHeight = 0
    private var isClosed = false
    private val videoByteList = LinkedList<ByteArray?>()
    var trackId:Int = 0
    var thread:Thread? =null




    private fun initMediaCodec() {
        yuv420spsrc = ByteArray(1920*1080*3/2)
        mWidth = CustomSurfaceView.mSize!!.width
        mHeight = CustomSurfaceView.mSize!!.height
        val size = mWidth * mHeight * 3 / 2
        log("width:${mWidth} --mHeight:${mHeight} 分配内存大小:${size}")
//        yuv420spsrc = ByteArray(size)
        //编码格式，AVC对应的是H264
        val mediaFormat =
            MediaFormat.createVideoFormat(MediaFormat.MIMETYPE_VIDEO_AVC, mWidth, mHeight)
        //YUV 420 对应的是图片颜色采样格式
        mediaFormat.setInteger(
            MediaFormat.KEY_COLOR_FORMAT,
            MediaCodecInfo.CodecCapabilities.COLOR_FormatYUV420Flexible
        )
        //比特率
        mediaFormat.setInteger(MediaFormat.KEY_BIT_RATE, 3000000)
        //帧率
        mediaFormat.setInteger(MediaFormat.KEY_FRAME_RATE, 30)
        //I 帧间隔
        mediaFormat.setInteger(MediaFormat.KEY_I_FRAME_INTERVAL, 1)
        try {
            mediaCodec = MediaCodec.createEncoderByType(MediaFormat.MIMETYPE_VIDEO_AVC)
            //创建生成MP4初始化对象
        } catch (e: IOException) {
            e.printStackTrace()
        }
        //进入配置状态
        mediaCodec.configure(mediaFormat, null, null, MediaCodec.CONFIGURE_FLAG_ENCODE)
        //进行生命周期执行状态
        mediaCodec.start()
    }


    fun startEncode() {
        initMediaCodec()
        // 开启线程轮询编码
        thread = innerThread()
        thread?.start()
    }

    fun stopEncode(){
        isClosed = true
            try {
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
            MuxerManager.getInstance().stop()
    }


    private fun innerThread():Thread{
        return object : Thread() {

            @RequiresApi(Build.VERSION_CODES.LOLLIPOP)
            override fun run() {
                val startTime = System.nanoTime()
                while (!isClosed) {
                    val bufferId = mediaCodec.dequeueInputBuffer(-1)
                    if (bufferId > 0) {
                        val byteBuffer = mediaCodec.getInputBuffer(bufferId)
                        val inputBuff = getInputBuffer() ?: break
                        byteBuffer?.put(inputBuff)
                        mediaCodec.queueInputBuffer(bufferId,0,inputBuff.size,(System.nanoTime() - startTime)/1000,0)
                    }
                    val info = MediaCodec.BufferInfo()
                    val outputId = mediaCodec.dequeueOutputBuffer(info,0)
                    if (outputId > 0){
                        val byteBuffer = mediaCodec.getOutputBuffer(outputId)
                        val byteArray = ByteArray(info.size)
                        byteBuffer?.get(byteArray)
                        byteBuffer?.position(info.offset)
                        byteBuffer?.limit(info.offset + info.size)
                        // 写入混合流中
                        if (MuxerManager.getInstance().isReady() && byteBuffer != null){
                            MuxerManager.getInstance().writeSampleData(trackId,byteBuffer,info)
//                            log("视频写入数据2:$trackId   buffer:$byteBuffer")
                        }
                        //释放output buffer
                        mediaCodec.releaseOutputBuffer(outputId, false)
                    } else if (outputId == MediaCodec.INFO_OUTPUT_FORMAT_CHANGED){
                        trackId = MuxerManager.getInstance().addTrack(mediaCodec.outputFormat)
                        MuxerManager.getInstance().start()
                    }
                }
                log("encode finished")
            }
        }
    }

    fun saveVideoByte(byteArray: ByteArray?){
        videoByteList.add(byteArray)
    }


    private fun getInputBuffer(): ByteArray? {
        val input = videoByteList.poll()
        NV21ToNV12(input, yuv420spsrc, mWidth, mHeight)
        return yuv420spsrc
    }

    private fun NV21ToNV12(
        nv21: ByteArray?,
        nv12: ByteArray?,
        width: Int,
        height: Int
    ) {
        if (nv21 == null || nv12 == null) {
            return
        }
        val framesize = width * height
        var i: Int
        var j: Int
        System.arraycopy(nv21, 0, nv12, 0, framesize)
        i = 0
        while (i < framesize) {
            nv12[i] = nv21[i]
            i++
        }
        j = 0
        while (j < framesize / 2) {
            nv12[framesize + j - 1] = nv21[j + framesize]
            j += 2
        }
        j = 0
        while (j < framesize / 2) {
            nv12[framesize + j] = nv21[j + framesize - 1]
            j += 2
        }
    }

}
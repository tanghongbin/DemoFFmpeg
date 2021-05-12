package com.example.democ.ffmpeg

import android.media.MediaCodec
import android.media.MediaFormat
import android.os.Bundle
import android.view.View
import androidx.appcompat.app.AppCompatActivity
import com.example.democ.R
import com.example.democ.audio.log
import com.example.democ.hwencoder.AudioConfiguration
import com.example.democ.hwencoder.HwEncoderHelper
import com.example.democ.interfaces.OutputEncodedDataListener
import com.example.democ.interfaces.OutputInitListener
import com.example.democ.render.FFmpegRender
import com.example.democ.rtmp.packer.rtmp.RtmpPacker
import com.example.common_base.utils.Constants
import kotlinx.android.synthetic.main.activity_rtmp_java_hw.*
import java.nio.ByteBuffer
import java.util.concurrent.atomic.AtomicInteger


/**
*
* author : tanghongbin
*
* date   : 2020/12/2 10:25
*
* desc   : 用android自带硬编码 mediacodec，混合录制视频
*
**/
class FFmpegRtmpJavaHwActivity : AppCompatActivity(),
    View.OnClickListener {

    lateinit var mMuxerHelper: HwEncoderHelper
    private val mRtmpSender by lazy { FFmpegRender() }
    private val mCount = AtomicInteger(0)
    private var startTime = 0L
    private val mPacker by lazy { RtmpPacker() }

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        setContentView(R.layout.activity_rtmp_java_hw)
        mMuxerHelper = HwEncoderHelper(mRtmpJavaSurface)
        mRtmpJavaStart.setOnClickListener(this)
        mRtmpJavaStop.setOnClickListener(this)
        mMuxerHelper.init()
        mMuxerHelper.setCaptureMode(HwEncoderHelper.CaptureMode.LIVE)
        init()
    }

    private fun init() {
        val audioListener = object : OutputEncodedDataListener {
            override fun outputData(
                byteBuffer: ByteBuffer,
                bufferInfo: MediaCodec.BufferInfo
            ) {
//                log("begin write audio")
                mPacker.onAudioData(byteBuffer,bufferInfo)
            }

            override fun outputFormatChanged(mediaFormat: MediaFormat) {
                mCount.addAndGet(1)
                log("audio outputformatchanged")
            }
        }

        val videoListener = object : OutputEncodedDataListener {
            override fun outputData(
                byteBuffer: ByteBuffer,
                bufferInfo: MediaCodec.BufferInfo
            ) {
//                log("begin write video")
                mPacker.onVideoData(byteBuffer,bufferInfo)
            }

            override fun outputFormatChanged(mediaFormat: MediaFormat) {
                mCount.addAndGet(1)
                log("video outputformatchanged")
            }
        }
        mMuxerHelper.setAudioOutputListener(audioListener)
        mMuxerHelper.setVideoOutputListener(videoListener)
        mMuxerHelper.setOutputInitListener(object : OutputInitListener {
            override fun init() {
            }

            override fun unInit() {

            }
        })
        mPacker.start()
        mPacker.initAudioParams(AudioConfiguration.DEFAULT_FREQUENCY, 16, false)
        mPacker.setPacketListener { data, packetType ->
//            LogUtils.log("has receive packet data:${data} type:${packetType}")
            mRtmpSender.native_sendPacketData(data,packetType)
        }
        mRtmpSender.native_rtmp_init(Constants.RTMP_ANDROID_PUSH)
    }


    override fun onResume() {
        super.onResume()
        mMuxerHelper.onResume()
    }

    override fun onStop() {
        super.onStop()
        mMuxerHelper.onStop()
    }

    override fun onDestroy() {
        super.onDestroy()
    }

    override fun onClick(v: View?) {
        when(v?.id){
            R.id.mRtmpJavaStart -> {
                log("开始推流")
                mMuxerHelper.startRecord()
                startTime = System.currentTimeMillis()
            }
            R.id.mRtmpJavaStop -> {
                log("停止推流")
                mMuxerHelper.stopRecord()
                mPacker.stop()
                mRtmpSender.native_rtmp_destroy()
            }
        }
    }
}

package com.example.democ.hwencoder

import android.media.MediaCodec
import android.media.MediaFormat
import androidx.appcompat.app.AppCompatActivity
import android.os.Bundle
import android.view.View
import com.example.camera.listener.CameraYUVDataListener
import com.example.camera.manager.CameraSurfaceManager
import com.example.democ.R
import com.example.democ.audio.*
import com.example.democ.audio.MuxerManager.Companion.getInstance
import com.example.democ.interfaces.OutputEncodedDataListener
import com.example.democ.interfaces.OutputInitListener
import kotlinx.android.synthetic.main.activity_f_fmpeg_hw_encode_muxer.*
import java.nio.ByteBuffer


/**
 *
 * author : tanghongbin
 *
 * date   : 2020/12/2 10:25
 *
 * desc   : 用android自带硬编码 mediacodec，混合录制视频
 *
 **/
class FFmpegHwEncodeMuxerActivity : AppCompatActivity(),
    View.OnClickListener {

    lateinit var mMuxerHelper: HwEncoderHelper
    private var audioTrackId = 0
    private var videoTrackId = 0

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        setContentView(R.layout.activity_f_fmpeg_hw_encode_muxer)
        mMuxerHelper = HwEncoderHelper(mHwSurface)
        mHwStart.setOnClickListener(this)
        mHwStop.setOnClickListener(this)
        mMuxerHelper.init()
        init()
    }

    private fun init() {
        val audioListener = object : OutputEncodedDataListener {
            override fun outputData(
                byteBuffer: ByteBuffer,
                bufferInfo: MediaCodec.BufferInfo
            ) {
                if (MuxerManager.Companion.getInstance().isReady()){
                    log("audio timestamp :${bufferInfo.presentationTimeUs}")
                    MuxerManager.getInstance()
                        .writeSampleData(audioTrackId, byteBuffer, bufferInfo)
                }
            }

            override fun outputFormatChanged(mediaFormat: MediaFormat) {
                audioTrackId = MuxerManager.getInstance().addTrack(mediaFormat)
                MuxerManager.getInstance().start()
            }
        }

        val videoListener = object : OutputEncodedDataListener {
            override fun outputData(
                byteBuffer: ByteBuffer,
                bufferInfo: MediaCodec.BufferInfo
            ) {
                if (MuxerManager.Companion.getInstance().isReady()){
                    log("video timestamp :${bufferInfo.presentationTimeUs}")
                    MuxerManager.getInstance()
                        .writeSampleData(videoTrackId, byteBuffer, bufferInfo);
                }
            }
            override fun outputFormatChanged(mediaFormat: MediaFormat) {
                videoTrackId = MuxerManager.getInstance().addTrack(mediaFormat)
                MuxerManager.getInstance().start()
            }
        }
        mMuxerHelper.setAudioOutputListener(audioListener)
        mMuxerHelper.setVideoOutputListener(videoListener)
        mMuxerHelper.setOutputInitListener(object : OutputInitListener {
            override fun init() {
                MuxerManager.getInstance().init()
            }

            override fun unInit() {
                MuxerManager.getInstance().stop()
            }
        })
    }

    override fun onResume() {
        super.onResume()
        mMuxerHelper.onResume()
    }

    override fun onStop() {
        super.onStop()
        mMuxerHelper.onStop()
    }

    override fun onClick(v: View?) {
        when (v?.id) {
            R.id.mHwStart -> {
                mMuxerHelper.startRecord()
            }
            R.id.mHwStop -> {
                mMuxerHelper.stopRecord()
            }
        }
    }
}

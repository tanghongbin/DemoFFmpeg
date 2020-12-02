package com.example.democ.hwencoder

import androidx.appcompat.app.AppCompatActivity
import android.os.Bundle
import android.view.View
import com.example.camera.listener.CameraYUVDataListener
import com.example.camera.manager.CameraSurfaceManager
import com.example.democ.R
import com.example.democ.audio.AudioEncoder
import com.example.democ.audio.AudioRecorder
import com.example.democ.audio.MuxerManager
import com.example.democ.audio.VideoEncoder
import kotlinx.android.synthetic.main.activity_f_fmpeg_hw_encode_muxer.*

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

    lateinit var mMuxerHelper:HwEncoderHelper

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        setContentView(R.layout.activity_f_fmpeg_hw_encode_muxer)
        mMuxerHelper = HwEncoderHelper(mHwSurface)
        mHwStart.setOnClickListener(this)
        mHwStop.setOnClickListener(this)
        mMuxerHelper.init()
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
        when(v?.id){
            R.id.mHwStart -> {
                mMuxerHelper.startRecord()
            }
            R.id.mHwStop -> {
                mMuxerHelper.stopRecord()
            }
        }
    }
}

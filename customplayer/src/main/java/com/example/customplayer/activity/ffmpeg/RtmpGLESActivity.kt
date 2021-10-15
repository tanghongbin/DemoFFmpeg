package com.example.customplayer.activity.ffmpeg

import android.os.Bundle
import androidx.appcompat.app.AppCompatActivity
import com.example.avutils.audio.recoder.AudioConfiguration
import com.example.avutils.audio.recoder.AudioRecorder
import com.example.common_base.utils.keepScreenOn
import com.example.common_base.utils.log
import com.example.common_base.utils.runAsyncTask
import com.example.customplayer.R
import com.example.customplayer.camera.CameraHelpr
import com.example.customplayer.player.CustomMediaController
import com.example.customplayer.util.camera2.Camera2FrameCallback
import com.example.customplayer.util.camera2.Camera2Wrapper
import kotlinx.android.synthetic.main.activity_gles_ffmpeg_muxer.*

/**
 * 用opengles 渲染camera数据,用x264 编码,用rtmp 推送流数据
 */
class RtmpGLESActivity : AppCompatActivity(), Camera2FrameCallback {
    private val mMuxer by lazy { CustomMediaController(2,3) }
    private val mCamera2Wrapper by lazy { Camera2Wrapper(this,this) }
    private val mAudioRecorder by lazy { AudioRecorder() }


    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        setContentView(R.layout.activity_gles_ffmpeg_muxer)
        keepScreenOn()
        muxerButton.setOnClickListener {
            runAsyncTask({
                val liveRtmpUrl = "rtmp://182.61.44.214:1935/live/windowsPush"
                mAudioRecorder.startCapture()
                mMuxer.native_configAudioParams(AudioConfiguration.DEFAULT_FREQUENCY,AudioConfiguration.DEFAULT_CHANNEL_COUNT)
                mMuxer.native_startEncode(liveRtmpUrl)
            })
        }
        mAudioRecorder.setOnAudioFrameCapturedListener { audioData, ret ->
            mMuxer.native_audioData(audioData,ret)
        }
        mGLESMuxerSurface.init(mMuxer,true)
        mGLESMuxerSurface.holder
//        ImageReader
        mCamera2Wrapper.startCamera()
    }

    override fun onDestroy() {
        mAudioRecorder.stopCapture()
        mCamera2Wrapper.stopCamera()
        mMuxer.native_OnDestroy()
        super.onDestroy()
    }

    override fun onPause() {
        super.onPause()
    }

    override fun onCaptureFrame(data: ByteArray?, width: Int, height: Int) {
        log("log onCaptureFrame data width:${width} height:${height} data:${data}")
    }

    override fun onPreviewFrame(data: ByteArray?, width: Int, height: Int) {
        log("log onPreviewFrame data width:${width} height:${height} data:${data}")
        if (data == null) return
        mMuxer.native_onCameraFrameDataValible(2,data)
    }

}
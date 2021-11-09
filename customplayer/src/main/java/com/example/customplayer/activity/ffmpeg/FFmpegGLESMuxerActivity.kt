package com.example.customplayer.activity.ffmpeg

import android.os.Bundle
import android.os.Parcel
import android.os.Parcelable
import androidx.appcompat.app.AppCompatActivity
import com.example.avutils.audio.recoder.AudioRecorder
import com.example.common_base.utils.log
import com.example.common_base.utils.runAsyncTask
import com.example.customplayer.R
import com.example.customplayer.camera.CameraHelpr
import com.example.customplayer.player.CustomMediaController
import com.example.customplayer.util.camera2.Camera2FrameCallback
import com.example.customplayer.util.camera2.Camera2Wrapper
import kotlinx.android.synthetic.main.activity_gles_ffmpeg_muxer.*

/**
 * 用opengles 渲染camera数据
 */
class FFmpegGLESMuxerActivity : AppCompatActivity(), Camera2FrameCallback {
    private val mMuxer by lazy { CustomMediaController(2,2) }
    private val mCamera2Wrapper by lazy { Camera2Wrapper(this,this) }
    private val mAudioRecorder by lazy { AudioRecorder() }

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        setContentView(R.layout.activity_gles_ffmpeg_muxer)
        muxerButton.setOnClickListener {
            runAsyncTask({
                mAudioRecorder.startCapture()
                mMuxer.native_setSpeed(1.0)
                mMuxer.native_startEncode()
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
        mMuxer.destroy()
        super.onDestroy()
    }

    override fun onPause() {
        super.onPause()
    }

    override fun onCaptureFrame(data: ByteArray?, width: Int, height: Int) {
//        log("log onCaptureFrame data width:${width} height:${height} data:${data}")
    }

    override fun onPreviewFrame(data: ByteArray?, width: Int, height: Int) {
//        log("log onPreviewFrame data width:${width} height:${height} data:${data}")
        if (data == null) return
        mMuxer.native_onCameraFrameDataValible(2,data)
    }

}

interface A1{
    fun test1();
}

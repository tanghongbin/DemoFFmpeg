package com.example.customplayer.activity.ffmpeg

import android.hardware.Camera
import android.media.ImageReader
import android.os.Bundle
import android.view.SurfaceHolder
import androidx.appcompat.app.AppCompatActivity
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
    private val mMuxer by lazy { CustomMediaController(2) }
    private val mCameraHelpr by lazy { CameraHelpr() }
    private val mCamera2Wrapper by lazy { Camera2Wrapper(this,this) }

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        setContentView(R.layout.activity_gles_ffmpeg_muxer)
        muxerButton.setOnClickListener {
            runAsyncTask({
                mMuxer.startTestEncode(2)
            })
        }
        mGLESMuxerSurface.init(mMuxer,true)
//        ImageReader
        mCamera2Wrapper.startCamera()
    }

    override fun onDestroy() {
        mMuxer.native_OnDestroy()
        super.onDestroy()
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
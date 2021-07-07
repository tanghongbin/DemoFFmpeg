package com.example.customplayer.activity.ffmpeg

import android.hardware.Camera
import android.os.Bundle
import android.view.SurfaceHolder
import androidx.appcompat.app.AppCompatActivity
import com.example.common_base.utils.log
import com.example.common_base.utils.runAsyncTask
import com.example.customplayer.R
import com.example.customplayer.camera.CameraHelpr
import com.example.customplayer.player.CustomMediaController
import kotlinx.android.synthetic.main.activity_f_fmpeg_muxer.*

class FFmpegMuxerActivity : AppCompatActivity(), SurfaceHolder.Callback {
    private val mMuxer by lazy { CustomMediaController(2) }
    private val mCameraHelpr by lazy { CameraHelpr() }

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        setContentView(R.layout.activity_f_fmpeg_muxer)
        muxerButton.setOnClickListener {
            runAsyncTask({
                mMuxer.native_startEncode()
            })
        }
//        mMuxerSurface.init(mMuxer)
        mCameraHelpr.setCameraType(Camera.CameraInfo.CAMERA_FACING_BACK)
        mMuxerSurface.holder.addCallback(this)
        mCameraHelpr.setPreviewCallback { data, camera ->
            log("打印设想数据:$data")
            mMuxer.native_onCameraFrameDataValible(1,data)
        }
//        mMuxer.setOnSurfaceCallListener(object : OnSurfaceCallListener{
//            override fun onSurfaceChanged(gl: GL10?, width: Int, height: Int) {
//
//            }
//            override fun onSurfaceCreated(gl: GL10?, config: EGLConfig?) {
//
//            }
//        })

    }

    override fun onDestroy() {
        mCameraHelpr.release()
        mMuxer.native_OnDestroy()
        super.onDestroy()
    }

    override fun surfaceChanged(holder: SurfaceHolder?, format: Int, width: Int, height: Int) {

    }

    override fun surfaceDestroyed(holder: SurfaceHolder?) {

    }

    override fun surfaceCreated(holder: SurfaceHolder?) {
        mCameraHelpr.openCamera(this@FFmpegMuxerActivity,holder)
    }

}
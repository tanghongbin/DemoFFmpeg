package com.example.customplayer.activity.ffmpeg

import androidx.appcompat.app.AppCompatActivity
import android.os.Bundle
import android.view.SurfaceHolder
import com.example.common_base.utils.log
import com.example.common_base.utils.runAsyncTask
import com.example.customplayer.R
import com.example.customplayer.camera.CameraHelpr
import com.example.customplayer.interfaces.OnSurfaceCallListener
import com.example.customplayer.player.CustomMediaController
import kotlinx.android.synthetic.main.activity_f_fmpeg_muxer.*
import javax.microedition.khronos.egl.EGLConfig
import javax.microedition.khronos.opengles.GL10

class FFmpegMuxerActivity : AppCompatActivity(){
    private val mMuxer by lazy { CustomMediaController(2) }
    private val mCameraHelpr by lazy { CameraHelpr() }

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        setContentView(R.layout.activity_f_fmpeg_muxer)
        muxerButton.setOnClickListener {
            runAsyncTask({
                mMuxer.startTestEncode(2)
            })
        }
        mMuxerSurface.init(mMuxer)
//        mMuxerSurface.holder.addCallback(this)
        mCameraHelpr.setPreviewCallback { data, camera ->
            log("打印设想数据:$data")
            mMuxer.native_onCameraFrameDataValible(data)
        }
        mMuxer.setOnSurfaceCallListener(object : OnSurfaceCallListener{
            override fun onSurfaceChanged(gl: GL10?, width: Int, height: Int) {

            }
            override fun onSurfaceCreated(gl: GL10?, config: EGLConfig?) {
                mCameraHelpr.openCamera(this@FFmpegMuxerActivity,null)
            }
        })

    }

    override fun onDestroy() {
        mMuxer.native_OnDestroy()
        super.onDestroy()
    }
}
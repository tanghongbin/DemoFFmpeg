package com.testthb.customplayer.activity.ffmpeg

import android.hardware.Camera
import android.os.Bundle
import android.view.SurfaceHolder
import androidx.appcompat.app.AppCompatActivity
import com.testthb.common_base.utils.log
import com.testthb.common_base.utils.runAsyncTask
import com.testthb.customplayer.R
import com.testthb.customplayer.camera.CameraHelpr
import com.testthb.customplayer.player.CustomMediaMuxer
import com.testthb.customplayer.player.MediaConstantsEnum
import kotlinx.android.synthetic.main.activity_f_fmpeg_muxer.*

class FFmpegMuxerActivity : AppCompatActivity(), SurfaceHolder.Callback {
    private val mMuxer by lazy { object : CustomMediaMuxer(){
        override fun getMuxerType(): MediaConstantsEnum {
            return MediaConstantsEnum.MUXER_RECORD_FFMPEG
        }
    } }
    private val mCameraHelpr by lazy { CameraHelpr() }

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        setContentView(R.layout.activity_f_fmpeg_muxer)
        muxerButton.setOnClickListener {
            runAsyncTask({
                mMuxer.native_initEncode()
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
        mMuxer.destroy()
        super.onDestroy()
    }

    override fun surfaceChanged(holder: SurfaceHolder, format: Int, width: Int, height: Int) {

    }

    override fun surfaceDestroyed(holder: SurfaceHolder) {

    }

    override fun surfaceCreated(holder: SurfaceHolder) {
        mCameraHelpr.openCamera(this@FFmpegMuxerActivity,holder)
    }


}
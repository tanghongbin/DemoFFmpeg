package com.example.democ.ffmpeg

import android.hardware.Camera
import android.os.Bundle
import android.view.SurfaceHolder
import androidx.appcompat.app.AppCompatActivity
import com.example.democ.R
import com.example.democ.audio.log
import com.example.democ.utils.CameraHelp
import com.libyuv.LibyuvUtil
import kotlinx.android.synthetic.main.ffmpeg_test_newcamerahelper.*

class FFmpegTestNewCameraHelperActivity : AppCompatActivity(), SurfaceHolder.Callback{


    private val mCameraHelp: CameraHelp = CameraHelp()
    private val isStop = false
    private val thread = Thread{

    }

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        setContentView(R.layout.ffmpeg_test_newcamerahelper)
        init()
    }


    private fun init() {
        mTestSurface.holder.addCallback(this)
        mCameraHelp.setPreviewCallback { nv21, camera ->
            val start = System.currentTimeMillis()
            val yuvI420 = ByteArray(nv21.size)
            val tempYuvI420 = ByteArray(nv21.size)

            val videoWidth = mCameraHelp.height
            val videoHeight = mCameraHelp.width

            LibyuvUtil.convertNV21ToI420(
                nv21,
                yuvI420,
                videoWidth,
                videoHeight
            )
            LibyuvUtil.compressI420(
                yuvI420, videoWidth, videoHeight, tempYuvI420,
                videoWidth, videoHeight, mCameraHelp.getRotation(mCameraHelp.isFrontCamera), mCameraHelp.isFrontCamera
            )
            log("总共耗时:${System.currentTimeMillis() - start}")
        }
        thread.run()
    }

    override fun surfaceChanged(holder: SurfaceHolder?, format: Int, width: Int, height: Int) {

    }

    override fun surfaceDestroyed(holder: SurfaceHolder?) {
        mCameraHelp.release()
    }

    override fun surfaceCreated(holder: SurfaceHolder?) {
        mCameraHelp.openCamera(
            this,
            Camera.CameraInfo.CAMERA_FACING_BACK,
            mTestSurface.holder
        )
    }

    override fun onResume() {
        super.onResume()
    }

    override fun onStop() {
        super.onStop()

    }

    override fun onDestroy() {
        super.onDestroy()
        mCameraHelp.release()
    }
}

package com.example.democ.ffmpeg

import android.graphics.Bitmap
import android.os.Bundle
import android.view.SurfaceHolder
import androidx.appcompat.app.AppCompatActivity
import com.example.camera.listener.CameraPictureListener
import com.example.camera.listener.CameraYUVDataListener
import com.example.camera.manager.CameraSurfaceManager
import com.example.democ.R
import com.example.democ.render.FFmpegRender
import kotlinx.android.synthetic.main.activity_f_fmpeg_encode_video.*
import kotlinx.android.synthetic.main.activity_f_fmpeg_encode_video.mSurface


class FFmpegEncodeVideoActivity : AppCompatActivity(),
    SurfaceHolder.Callback, CameraPictureListener, CameraYUVDataListener {

    lateinit var mRender:FFmpegRender
    lateinit var mCameraHelper: CameraSurfaceManager
    lateinit var camera: android.hardware.Camera
    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        setContentView(R.layout.activity_f_fmpeg_encode_video)
        mCameraHelper = CameraSurfaceManager(mSurface)
        mCameraHelper.setCameraPictureListener(this)
        mSurface.setCameraYUVDataListener(this)
        mSurface.setmHolderCall(this)
        mRender = FFmpegRender()
        mReadFile.setOnClickListener {
            mRender.native_testReadFile()
        }
    }

    var isSet = false

    override fun onResume() {
        super.onResume()
        mCameraHelper.onResume()
    }

    override fun onStop() {
        super.onStop()
        mCameraHelper.onStop()
    }


    override fun onBackPressed() {

        super.onBackPressed()
    }

    override fun onDestroy() {
        super.onDestroy()
    }

    override fun surfaceChanged(holder: SurfaceHolder?, format: Int, width: Int, height: Int) {
        mRender.native_OnSurfaceChanged(mSurface.cameraUtil.cameraWidth,mSurface.cameraUtil.cameraHeight)
        mRender.native_videoEncodeInit()
    }

    override fun surfaceDestroyed(holder: SurfaceHolder?) {
        mRender.native_videoEncodeUnInit()
    }

    override fun surfaceCreated(holder: SurfaceHolder?) {

//        runAsyncTask({
//
//        },{
//
//        })

    }

    override fun onPictureBitmap(btmp: Bitmap?) {

    }

    var index = 0;

    override fun onCallback(srcData: ByteArray?) {
        val windowWidth = mSurface.cameraUtil.cameraWidth
        val windowHeight = mSurface.cameraUtil.cameraHeight
        val scaleHeight = mSurface.cameraUtil.cameraWidth
        val scaleWidth = mSurface.cameraUtil.cameraHeight
        val mOrientation = mSurface.cameraUtil.orientation
        val bytes = ByteArray(windowHeight * windowWidth * 3 / 2)
        val start = System.currentTimeMillis()
//        YuvUtil.yuvCompress(
//            srcData,
//            cameraWidth,
//            windowHeight,
//            bytes,
//            scaleHeight,
//            scaleWidth,
//            3,
//            mOrientation,
//            mOrientation == 270
//        )

        mRender.native_encodeFrame(bytes)
    }
}

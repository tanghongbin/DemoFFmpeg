package com.example.democ.ffmpeg

import android.graphics.Bitmap
import android.hardware.Camera
import android.os.Bundle
import android.view.SurfaceHolder
import androidx.appcompat.app.AppCompatActivity
import com.example.camera.listener.CameraPictureListener
import com.example.camera.listener.CameraYUVDataListener
import com.example.camera.manager.CameraSurfaceManager
import com.example.democ.R
import kotlinx.android.synthetic.main.activity_f_fmpeg_open_g_l.*

class FFmpegTextureActivity : AppCompatActivity(), SurfaceHolder.Callback, Camera.PreviewCallback,
    CameraPictureListener, CameraYUVDataListener {


//    private lateinit var mCameraHelper:CameraHelper
private lateinit var mCameraHelper:CameraSurfaceManager
    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        setContentView(R.layout.activity_f_fmpeg_open_g_l)
        init()
    }

    var isSet = false

    private fun init() {
//        mCameraHelper = CameraSurfaceManager(mSurface)
//        mCameraHelper.setCameraPictureListener(this)
//        mSurface.setCameraYUVDataListener(this)
//        mOpenGlEs.init(FFmpegOpenGLRender())
//
//        mCameraHelper = CameraHelper(this, mSurface, this)
//        mCameraHelper.init(1920,1080)
////        mCameraHelper.init()
//        mCameraHelper.setPreviewCallback(this)
    }

    override fun surfaceChanged(holder: SurfaceHolder?, format: Int, width: Int, height: Int) {

    }

    override fun surfaceDestroyed(holder: SurfaceHolder?) {

    }

    override fun surfaceCreated(holder: SurfaceHolder?) {

    }

    override fun onResume() {
        super.onResume()
        mCameraHelper.onResume()
    }

    override fun onStop() {
        super.onStop()
        mCameraHelper.onStop()
    }

    override fun onPreviewFrame(srcData: ByteArray?, camera: Camera?) {

    }

    override fun onPictureBitmap(btmp: Bitmap?) {
        mGlImage.setImageBitmap(btmp)
    }

    override fun onCallback(srcData: ByteArray?) {
//        val windowWidth = mSurface.cameraUtil.cameraWidth
//        val windowHeight = mSurface.cameraUtil.cameraHeight
//        val scaleHeight = mSurface.cameraUtil.cameraWidth
//        val scaleWidth = mSurface.cameraUtil.cameraHeight
//        val mOrientation = mSurface.cameraUtil.orientation

        if (!isSet) {
//            isSet = true

//            val bytes = ByteArray(windowHeight * windowWidth * 3 / 2)

//            YuvUtil.yuvCompress(
//                srcData,
//                windowWidth,
//                windowHeight,
//                bytes,
//                scaleHeight,
//                scaleWidth,
//                3,
//                mOrientation,
//                mOrientation == 270
//            )
//
//            val dstData = ByteArray(windowHeight * windowWidth * 3 / 2)
//            log("prepare yuv420 to nv21")
//            YuvUtil.yuvI420ToNV21(
//                bytes,
//                scaleWidth,
//                scaleHeight,
//                dstData
//            )
//            mGlImage.setImageBitmap(
//                getBitmapFromYuvdata(
//                    dstData,
//                    scaleWidth,
//                    scaleHeight
//                )
//            )
        }


    }
}

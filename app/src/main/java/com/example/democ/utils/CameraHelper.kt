package com.example.democ.utils

import android.app.Activity
import android.graphics.ImageFormat
import android.hardware.Camera
import android.util.DisplayMetrics
import android.view.Surface
import android.view.SurfaceHolder
import android.view.SurfaceView
import android.view.WindowManager
import androidx.appcompat.app.AppCompatActivity
import com.example.democ.audio.log

/**
 *
 * author : tanghongbin
 *
 * date   : 2020/10/26 14:29
 *
 * desc   : 打开相机并且预览回调数据
 *
 **/
class CameraHelper(
    val activity: Activity,
    val surface: SurfaceView,
    val call: SurfaceHolder.Callback
) : SurfaceHolder.Callback,
    Camera.PreviewCallback {
    private lateinit var camera: Camera
    private var mRotation = 0
    private var mWidth: Int = 0
    private var mHeight: Int = 0
    private var mCameraId = 1
    private var mCall: Camera.PreviewCallback? = null
    private var mHolder: SurfaceHolder? = null

    fun getVideoWidth(): Int {
        return mWidth
    }

    fun getVideoHeight(): Int {
        return mHeight
    }

    fun init(width: Int, height: Int) {
        surface.holder.addCallback(this)
        mWidth = width
        mHeight = height
        if (mWidth <= 0 || mHeight <= 0) {
            throw IllegalArgumentException("Illegal Argument width or height is 0")
        }
    }

    fun setPreviewCallback(call: Camera.PreviewCallback) {
        mCall = call
    }

    private fun initCameraParams(camera: Camera?) {
        val parameters = camera?.getParameters();
        parameters?.let {
            parameters.previewFormat = ImageFormat.NV21
            setPreviewSize(parameters)
            setCameraDisplayOrientation(activity, mCameraId, camera)
            camera.parameters = parameters
        }
        camera?.setPreviewCallback(this)
    }

    private fun setPreviewSize(parameters: Camera.Parameters) {
        val supportedPreviewSizes =
            parameters.supportedPreviewSizes
        var size = supportedPreviewSizes[0]
        //select the best resolution of camera
        var m = Math.abs(size.height * size.width - mWidth * mHeight)
        supportedPreviewSizes.removeAt(0)
        for (next in supportedPreviewSizes) {
            val n = Math.abs(next.height * next.width - mWidth * mHeight)
            if (n < m) {
                m = n
                size = next
            }
        }
        mWidth = size.width
        mHeight = size.height
        log("预览大小   width:${mWidth}  -- height:${mHeight}")
        parameters.setPreviewSize(mWidth, mHeight)
    }

    private fun rotation90(dataArray: ByteArray) {
//        var index = 0
//        val ySize: Int = mWidth * mHeight
//        val uvHeight: Int = mHeight / 2
//        //back camera rotate 90 deree
//        if (mCameraId == Camera.CameraInfo.CAMERA_FACING_BACK) {
//            for (i in 0 until mWidth) {
//                for (j in mHeight - 1 downTo 0) {
//                    bytes!![index++] = dataArray[mWidth * j + i]
//                }
//            }
//            var i = 0
//            while (i < mWidth) {
//                for (j in uvHeight - 1 downTo 0) { // v
//                    bytes!![index++] = dataArray[ySize + mWidth * j + i]
//                    // u
//                    bytes!![index++] = dataArray[ySize + mWidth * j + i + 1]
//                }
//                i += 2
//            }
//        } else { //rotate 90 degree
//            for (i in 0 until mWidth) {
//                var nPos: Int = mWidth - 1
//                for (j in 0 until mHeight) {
//                    bytes!![index++] = dataArray[nPos - i]
//                    nPos += mWidth
//                }
//            }
//            //u v
//            var i = 0
//            while (i < mWidth) {
//                var nPos: Int = ySize + mWidth - 1
//                for (j in 0 until uvHeight) {
//                    bytes!![index++] = dataArray[nPos - i - 1]
//                    bytes!![index++] = dataArray[nPos - i]
//                    nPos += mWidth
//                }
//                i += 2
//            }
//        }
    }


    private fun startPreview() {
        camera = android.hardware.Camera.open(mCameraId)
        initCameraParams(camera)
        camera.setPreviewDisplay(mHolder)
        camera.startPreview()
    }

    fun release() {
        camera.setPreviewCallback(null)
        camera.stopPreview()
        camera.release()
    }

    private fun setCameraDisplayOrientation(
        activity: Activity,
        cameraId: Int, camera: Camera
    ) {
        val info = Camera.CameraInfo()
        Camera.getCameraInfo(cameraId, info)
        mRotation = activity.windowManager.defaultDisplay
            .rotation
        var degrees = 0
        when (mRotation) {
            Surface.ROTATION_0 -> degrees = 0
            Surface.ROTATION_90 -> degrees = 90
            Surface.ROTATION_180 -> degrees = 180
            Surface.ROTATION_270 -> degrees = 270
        }
        var result: Int
        if (info.facing == Camera.CameraInfo.CAMERA_FACING_FRONT) {
            result = (info.orientation + degrees) % 360
            result = (360 - result) % 360 // compensate the mirror
        } else { // back-facing
            result = (info.orientation - degrees + 360) % 360
        }
        camera.setDisplayOrientation(result)
    }

    override fun surfaceChanged(holder: SurfaceHolder?, format: Int, width: Int, height: Int) {
        log("java surfaceChanged")
        call.surfaceChanged(holder, format, width, height)
    }

    override fun surfaceDestroyed(holder: SurfaceHolder?) {
        log("java surfaceDestroyed")
        release()
        call.surfaceDestroyed(holder)
    }

    override fun surfaceCreated(holder: SurfaceHolder?) {
        mHolder = holder
        startPreview()
        call.surfaceCreated(holder)
        log("java surfaceCreated")
    }

    override fun onPreviewFrame(data: ByteArray?, camera: Camera?) {
        var dataResult = data
        when (mRotation) {
            Surface.ROTATION_0 -> {
//                dataResult = rotation90(data!!)
            }
            Surface.ROTATION_90, Surface.ROTATION_270 -> {
            }
            else -> {
            }
        }
        mCall?.onPreviewFrame(dataResult, camera)
    }
}
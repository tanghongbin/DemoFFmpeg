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
import com.libyuv.util.YuvUtil

/**
 *
 * author : tanghongbin
 *
 * date   : 2020/10/26 14:29
 *
 * desc   : 打开相机并且预览回调数据
 *
 **/
@Deprecated("不用了，有更好的")
class CameraHelper(
    val activity: Activity,
    val surface: SurfaceView,
    val call: SurfaceHolder.Callback
) : SurfaceHolder.Callback,
    Camera.PreviewCallback {
    lateinit var camera: Camera
    private var mRotation = 0
    private var bytes: ByteArray? = null
    private var buffer: ByteArray? = null
    private var mWidth: Int = 0
    private var mHeight: Int = 0
    private var mScaleWidth: Int = 0
    private var mScaleHeight: Int = 0
    private var mCameraId = 1
    private var mCall: Camera.PreviewCallback? = null
    private var mHolder: SurfaceHolder? = null
    private var mOrientation = 0



    fun getOrientation():Int{
        return mOrientation
    }

    fun getScaleWidth(): Int {
        return mScaleWidth
    }

    fun getScaleHeight(): Int {
        return mScaleHeight
    }

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
        mScaleWidth = mHeight
        mScaleHeight = mWidth
        if (mWidth <= 0 || mHeight <= 0) {
            throw IllegalArgumentException("Illegal Argument width or height is 0")
        }
    }

    fun setPreviewCallback(call: Camera.PreviewCallback) {
        mCall = call
    }

    private fun initCameraParams(camera: Camera?) {
        val parameters = camera?.getParameters();
        val manager = activity.getSystemService(AppCompatActivity.WINDOW_SERVICE) as WindowManager
        val displayMetrics = DisplayMetrics()
        manager.defaultDisplay.getMetrics(displayMetrics)
        log("打印窗口的宽:${displayMetrics.widthPixels} 高:${displayMetrics.heightPixels}")
        parameters?.let {
            parameters.previewFormat = ImageFormat.NV21
            setPreviewSize(parameters)
            setCameraDisplayOrientation(activity, mCameraId, camera)
            camera.parameters = parameters
        }

        buffer = ByteArray(mWidth * mHeight * 3 / 2)
        bytes = ByteArray(buffer!!.size)
//
        camera?.addCallbackBuffer(buffer)
        camera?.setPreviewCallbackWithBuffer(this)


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


    fun startPreview() {
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

    fun setCameraDisplayOrientation(
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
        mOrientation = result
        camera.setDisplayOrientation(result)
    }

    override fun surfaceChanged(holder: SurfaceHolder?, format: Int, width: Int, height: Int) {
        log("java surfaceChanged")
        call.surfaceChanged(holder, format, width, height)
    }

    override fun surfaceDestroyed(holder: SurfaceHolder?) {
        log("java surfaceDestroyed")
        call.surfaceDestroyed(holder)
    }

    override fun surfaceCreated(holder: SurfaceHolder?) {
        mHolder = holder
        startPreview()
        call.surfaceCreated(holder)
        log("java surfaceCreated")
    }

    override fun onPreviewFrame(srcData: ByteArray?, camera: Camera?) {
//        log("打印旋转角度:${mRotation}")
//        log("打印原数组大小:${srcData?.size}  现在数组大小:${bytes?.size}")
//        var dataResult = ByteArray(buffer!!.size)
        log("mRotation:${mRotation}  mOrientation:${mOrientation}")
        when (mRotation) {
            Surface.ROTATION_0 -> {
//                log("角度为0")
//                YuvUtil.yuvCompress(
//                    srcData,
//                    mWidth,
//                    mHeight,
//                    bytes,
//                    mHeight,
//                    mWidth,
//                    2,
//                    90,
//                    false
//                )
                YuvUtil.yuvCompress(
                    srcData,
                    mWidth,
                    mHeight,
                    bytes,
                    mScaleHeight,
                    mScaleWidth,
                    3,
                    mOrientation,
                    mOrientation == 270
                )
//                YuvUtil.yuvNV21ToI420(data,mWidth,mHeight,bytes)
//                YuvUtil.yuvNV21ToI420AndRotate(srcData,mWidth,mHeight,bytes,270)
                log("打印本身已经旋转的角度:${mOrientation}")
            }
            Surface.ROTATION_90, Surface.ROTATION_270 -> {
            }
            else -> {
            }
        }
//        mCall?.onPreviewFrame(dataResult, camera)
        mCall?.onPreviewFrame(bytes, camera)
        camera!!.addCallbackBuffer(buffer)
    }

}
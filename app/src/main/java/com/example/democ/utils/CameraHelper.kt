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
import kotlinx.android.synthetic.main.activity_f_fmpeg_encode_video.*

/**
*
* author : tanghongbin
*
* date   : 2020/10/26 14:29
*
* desc   : 打开相机并且预览回调数据
*
**/
class CameraHelper(val activity: Activity,val surface: SurfaceView,val call:SurfaceHolder.Callback) : SurfaceHolder.Callback {
    lateinit var camera:Camera
    fun init(){
        camera = android.hardware.Camera.open(1)
//        initCameraParams(camera)
        surface.holder.addCallback(this)
        camera = android.hardware.Camera.open(1)
//        initCameraParams(camera)
        setCameraDisplayOrientation(activity,1,camera)
    }

    fun setPreviewCallback(call:Camera.PreviewCallback){
        camera.setPreviewCallback(call)
    }

    private fun initCameraParams(camera: Camera?) {
        var parameters = camera?.getParameters();
        val manager = activity.getSystemService(AppCompatActivity.WINDOW_SERVICE) as WindowManager
        val displayMetrics = DisplayMetrics()
        manager.defaultDisplay.getMetrics(displayMetrics)
        log("打印窗口的宽:${displayMetrics.widthPixels} 高:${displayMetrics.heightPixels}")
        parameters?.let {
            parameters.setFlashMode("off") // 无闪光灯
            parameters.setWhiteBalance(Camera.Parameters.WHITE_BALANCE_AUTO)
            parameters.setSceneMode(Camera.Parameters.SCENE_MODE_AUTO)
            parameters.setFocusMode(Camera.Parameters.FOCUS_MODE_AUTO)
            parameters.setPreviewFormat(ImageFormat.NV21)
            parameters.setPictureSize(displayMetrics.widthPixels, displayMetrics.heightPixels);
            parameters.setPreviewSize(displayMetrics.widthPixels, displayMetrics.heightPixels)
            //这两个属性 如果这两个属性设置的和真实手机的不一样时，就会报错
            camera?.setParameters(parameters)
        }

    }

    fun release(){
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
        val rotation = activity.windowManager.defaultDisplay
            .rotation
        var degrees = 0
        when (rotation) {
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
        call.surfaceChanged(holder,format,width,height)
    }

    override fun surfaceDestroyed(holder: SurfaceHolder?) {
        log("java surfaceDestroyed")
        call.surfaceDestroyed(holder)
    }

    override fun surfaceCreated(holder: SurfaceHolder?) {
        camera.setPreviewDisplay(holder)
        camera.startPreview()
        call.surfaceCreated(holder)
        log("java surfaceCreated")
    }
}
package com.example.democ

import android.app.Activity
import android.content.Context
import android.graphics.*
import android.hardware.Camera
import android.hardware.Camera.CameraInfo
import android.util.AttributeSet
import android.view.Display
import android.view.Surface
import android.view.TextureView
import com.example.democ.audio.log
import java.io.ByteArrayOutputStream
import java.io.IOException

class CustomSurfaceView @JvmOverloads constructor(
    context: Context, attrs: AttributeSet? = null, defStyleAttr: Int = 0
) : TextureView(context, attrs, defStyleAttr),TextureView.SurfaceTextureListener,Camera.PreviewCallback {

    companion object{
        var mSize: Camera.Size? = null
    }

    var mCamera:Camera? = null
    init {
        surfaceTextureListener = this
    }
    private var callBack:(Bitmap?) -> Unit = {}

    private var parameters: Camera.Parameters? = null



    override fun onPreviewFrame(data: ByteArray?, camera: Camera?) {
        log("帧数据:${data}")
    }

    private var hasBuild = false


    fun setCallback(block:(Bitmap?) -> Unit ){
        callBack = block
    }

    override fun onSurfaceTextureSizeChanged(surface: SurfaceTexture?, width: Int, height: Int) {

    }

    override fun onSurfaceTextureUpdated(surface: SurfaceTexture?) {

    }

    override fun onSurfaceTextureDestroyed(surface: SurfaceTexture?): Boolean {
        mCamera?.stopPreview()
        mCamera?.release()
        return true
    }

    override fun onSurfaceTextureAvailable(surface: SurfaceTexture?, width: Int, height: Int) {
        initCamera()
    }


    fun initCamera() {
        mCamera = Camera.open(1)
        mCamera?.setPreviewCallback(this)
        setCameraDisplayOrientation()
        if (parameters == null) {
            parameters = mCamera?.parameters
        }
        parameters?.previewFormat = ImageFormat.NV21
        val mSizeList: List<Camera.Size>? =
            parameters?.supportedPreviewSizes
        mSize = mSizeList!![0]
        parameters?.setPreviewSize(
            mSize?.width ?: 0,
            mSize?.height ?: 0
        )
        mCamera?.parameters = parameters
        try {
            mCamera?.setPreviewTexture(surfaceTexture)
            mCamera?.startPreview()
        } catch (e: IOException) {
            e.printStackTrace()
        }
    }

    private fun setCameraDisplayOrientation() {
        val mDisplay: Display = (context as Activity).windowManager.defaultDisplay
        val orientation = mDisplay.orientation
        val info = CameraInfo()
        Camera.getCameraInfo(1, info)
        var degrees = 0
        when (orientation) {
            Surface.ROTATION_0 -> degrees = 0
            Surface.ROTATION_90 -> degrees = 90
            Surface.ROTATION_180 -> degrees = 180
            Surface.ROTATION_270 -> degrees = 270
            else -> {
            }
        }
        var result: Int
        if (info.facing == CameraInfo.CAMERA_FACING_FRONT) {
            result = (info.orientation + degrees) % 360
            result = (360 - result) % 360 // compensate the mirror
        } else { // back-facing
            result = (info.orientation - degrees + 360) % 360
        }
        mCamera!!.setDisplayOrientation(result)
        log(" orientation: $orientation")
    }


    fun onDestroy() {
        if (mCamera != null) {
            mCamera!!.setPreviewTexture(null)
            mCamera!!.setPreviewCallback(null)
            mCamera!!.stopPreview()
            mCamera!!.release()
            mCamera = null
        }
    }


}

fun getBitmapFromYuvdata(data: ByteArray?,width: Int,height: Int):Bitmap?{
//        if (hasBuild) return
//        hasBuild = true
    val yuvImage = YuvImage(data,ImageFormat.NV21,width,height,null)
    val out = ByteArrayOutputStream()
    val result = yuvImage.compressToJpeg(Rect(0,0,width,height),100,out)
    if (result){
        val array = out.toByteArray()
        val bitmap = BitmapFactory.decodeByteArray(array,0,array.size)
        out.close()
        return bitmap
    }
    return null
}
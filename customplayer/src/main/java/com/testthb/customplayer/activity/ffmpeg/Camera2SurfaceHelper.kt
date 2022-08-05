package com.testthb.customplayer.activity.ffmpeg

import android.content.Context
import android.graphics.SurfaceTexture
import com.testthb.common_base.utils.log
import com.testthb.customplayer.interfaces.OnDrawListener
import com.testthb.customplayer.interfaces.OnOESTextureListener
import com.testthb.customplayer.player.CustomMediaRecorder
import com.testthb.customplayer.util.camera2.Camera2FrameCallback
import com.testthb.customplayer.util.camera2.Camera2Wrapper

/***
 * 简化surfacetexture的生成与调用
 */
class Camera2SurfaceHelper(context: Context,private val mMuxer: CustomMediaRecorder) :
    Camera2FrameCallback {
    private var mSurfaceTexture:SurfaceTexture? = null
    private val mCamera2Wrapper by lazy { Camera2Wrapper(context,this) }
    private var isDestroyed = false
    fun init() {
        mMuxer.setOnOESTextureListener(object : OnOESTextureListener {
            override fun onTextureCall(textureId: Int) {
                log("打印获取的纹理id:${textureId}")
                mSurfaceTexture = SurfaceTexture(textureId)
                mCamera2Wrapper.startCamera(mSurfaceTexture)
            }
        })
        mMuxer.setOnDrawListener(object : OnDrawListener {
            override fun onDraw() {
                if (isDestroyed) return
                mSurfaceTexture?.updateTexImage()
                val floatMatrix = FloatArray(16)
                mSurfaceTexture?.getTransformMatrix(floatMatrix)
//                android.opengl.Matrix.rotateM(floatMatrix,0,mRorate.toFloat(),0f,0.0f,1.0f)
                mMuxer.native_updateMatrix(floatMatrix)
            }
        })
    }

    fun onDestroy(){
        isDestroyed = true
        mCamera2Wrapper.stopCamera()
    }

    fun release(){
//        mSurfaceTexture?.release()
//        mSurfaceTexture = null
    }

    override fun onPreviewFrame(data: ByteArray?, width: Int, height: Int) {

    }

    override fun onCaptureFrame(data: ByteArray?, width: Int, height: Int) {

    }

}
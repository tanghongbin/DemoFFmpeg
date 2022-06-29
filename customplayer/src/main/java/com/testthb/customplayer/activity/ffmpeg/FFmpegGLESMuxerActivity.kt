package com.testthb.customplayer.activity.ffmpeg

import android.content.Context
import android.graphics.Camera
import android.graphics.SurfaceTexture
import android.hardware.Camera.CameraInfo
import android.os.Build
import android.os.Bundle
import android.view.View
import android.view.WindowManager
import android.widget.SeekBar
import androidx.annotation.RequiresApi
import androidx.appcompat.app.AppCompatActivity
import com.testthb.avutils.audio.recoder.AudioRecorder
import com.testthb.common_base.utils.log
import com.testthb.common_base.utils.runAsyncTask
import com.testthb.customplayer.R
import com.testthb.customplayer.interfaces.OnDrawListener
import com.testthb.customplayer.interfaces.OnErrorListener
import com.testthb.customplayer.interfaces.OnOESTextureListener
import com.testthb.customplayer.player.CustomMediaController
import com.testthb.customplayer.util.camera2.Camera2FrameCallback
import com.testthb.customplayer.util.camera2.Camera2Wrapper
import kotlinx.android.synthetic.main.activity_gles_ffmpeg_muxer.*

/**
 * 用opengles 渲染camera数据
 */
class FFmpegGLESMuxerActivity : AppCompatActivity(), Camera2FrameCallback {
    private val mMuxer by lazy { CustomMediaController(2,2) }
    private val mCamera2Wrapper by lazy { Camera2Wrapper(this,this) }
    private val mAudioRecorder by lazy { AudioRecorder() }
    private var mSurfaceTexture:SurfaceTexture? = null
    private var mRorate = 0

    @RequiresApi(Build.VERSION_CODES.O)
    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        setContentView(R.layout.activity_gles_ffmpeg_muxer)
        muxerButton.setOnClickListener {
            runAsyncTask({
                mAudioRecorder.startCapture()
                mMuxer.native_setSpeed(1.0)
                mMuxer.native_startEncode()
            })
        }

        mRotate.setOnClickListener {
            val info = CameraInfo()
            info.orientation
            android.hardware.Camera.getCameraInfo(0,info)
            val rotation: Int = (getSystemService(Context.WINDOW_SERVICE)as WindowManager).getDefaultDisplay()
                .getRotation()
            log("查看camera方向:${info.orientation}  屏幕旋转方向:${rotation}")
        }
        mAudioRecorder.setOnAudioFrameCapturedListener { audioData, ret ->
            mMuxer.native_audioData(audioData,ret)
        }
        mGLESMuxerSurface.init(mMuxer,true)
        mMuxer.setOnOESTextureListener(object : OnOESTextureListener{
            override fun onTextureCall(textureId: Int) {
                log("打印获取的纹理id:${textureId}")
                mSurfaceTexture = SurfaceTexture(textureId)
                mCamera2Wrapper.startCamera(mSurfaceTexture)
            }
        })
        mMuxer.setOnDrawListener(object : OnDrawListener{
            override fun onDraw() {
                mSurfaceTexture?.updateTexImage()
                val floatMatrix = FloatArray(16)
                mSurfaceTexture?.getTransformMatrix(floatMatrix)
//                android.opengl.Matrix.rotateM(floatMatrix,0,mRorate.toFloat(),0f,0.0f,1.0f)
                mMuxer.native_updateMatrix(floatMatrix)
            }
        })
        mMuxer.setOnErrorListener(object : OnErrorListener{
            override fun onError(code: Int, str: String) {
                log("打印错误code:${code}  msg:${str}")
            }
        })
        mseekbar.min = 0
        mseekbar.max = 360
        mseekbar.visibility = View.VISIBLE
        mseekbar.setOnSeekBarChangeListener(object : SeekBar.OnSeekBarChangeListener {
            override fun onProgressChanged(seekBar: SeekBar?, progress: Int, fromUser: Boolean) {
                mRorate = progress
            }
            override fun onStartTrackingTouch(seekBar: SeekBar?) {}
            override fun onStopTrackingTouch(seekBar: SeekBar?) {}
        })
//        ImageReader
//        mCamera2Wrapper.startCamera()
    }

    override fun onDestroy() {
        mAudioRecorder.stopCapture()
        mCamera2Wrapper.stopCamera()
        mMuxer.destroy()
        super.onDestroy()
    }

    override fun onPause() {
        super.onPause()
    }

    override fun onCaptureFrame(data: ByteArray?, width: Int, height: Int) {
//        log("log onCaptureFrame data width:${width} height:${height} data:${data}")
    }

    override fun onPreviewFrame(data: ByteArray?, width: Int, height: Int) {
//        log("log onPreviewFrame data width:${width} height:${height} data:${data}")
        if (data == null) return
        log("更新surfacetexture:${mSurfaceTexture}")
        mMuxer.native_onCameraFrameDataValible(5,data)
    }

}

interface A1{
    fun test1();
}

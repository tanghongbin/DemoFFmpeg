package com.example.democ.hwencoder

import android.hardware.Camera
import android.view.Surface
import android.view.SurfaceHolder
import com.example.camera.listener.CameraYUVDataListener
import com.example.camera.manager.CameraSurfaceManager
import com.example.camera.manager.CameraSurfaceView
import com.example.democ.audio.*
import com.example.democ.interfaces.OutputEncodedDataListener
import com.example.democ.interfaces.OutputInitListener
import com.example.democ.interfaces.SingleGetDataInterface
import java.lang.IllegalStateException
import java.lang.NullPointerException

/**
 *
 * author : tanghongbin
 *
 * date   : 2020/12/2 10:58
 *
 * desc   : 录制mp4帮助类，硬编码
 *
 **/
class HwEncoderHelper(private val cameraSurfaceView: CameraSurfaceView) : CameraYUVDataListener,
    SurfaceHolder.Callback {

    private var mTestStart = false
    private var mInited = false
    private val mAudioRecorder by lazy { AudioRecorder() }
    private val player by lazy { CustomAudioTrack() }
    private val mAudioEncoder by lazy { AudioEncoder() }
    private val decoder by lazy { AudioDecoder() }
    private val mVideoEncoder by lazy { VideoEncoder() }
    private lateinit var mCameraSurfaceManager: CameraSurfaceManager
    private var audioOutputListener:OutputEncodedDataListener? = null
    private var videoOutputListener:OutputEncodedDataListener? = null
    private var initListener:OutputInitListener? = null
    private var mCaptureMode = CaptureMode.RECORD

    fun setCaptureMode(mode:CaptureMode){
        mCaptureMode = mode
    }

    fun init() {
        if (mInited) return
        mCameraSurfaceManager = CameraSurfaceManager(cameraSurfaceView)
        mCameraSurfaceManager.mCameraUtil.currentCameraType =
            Camera.CameraInfo.CAMERA_FACING_FRONT
        cameraSurfaceView.setCameraYUVDataListener(this)
        cameraSurfaceView.setmHolderCall(this)
        mInited = true
    }



    fun setAudioOutputListener(listener:OutputEncodedDataListener?){
        audioOutputListener = listener
    }

    fun setVideoOutputListener(listener:OutputEncodedDataListener?){
        videoOutputListener = listener
    }

    fun setOutputInitListener(listener:OutputInitListener?){
        initListener = listener
    }

    fun startRecord() {
        if (!mInited) throw IllegalStateException("you must call init() before startRecord()")
        if (audioOutputListener == null || videoOutputListener == null || initListener == null)
            throw NullPointerException("audioOutputListener or initListener can't be null")
        if (mTestStart) return
        initListener?.init()
        mAudioRecorder.setOnAudioFrameCapturedListener {
            mAudioEncoder.encode(it)
        }
        mAudioRecorder.startCapture()
        mAudioEncoder.setOutputListener(audioOutputListener)
        mVideoEncoder.setOutputListener(videoOutputListener)
        mAudioEncoder.setCaptureMode(mCaptureMode)
        mVideoEncoder.setCaptureMode(mCaptureMode)
        mAudioEncoder.startEncode()
        mVideoEncoder.startEncode()
        mTestStart = true
        log("already start record")
    }

    fun stopRecord() {
        mTestStart = false

        mVideoEncoder.stopEncode()
        mAudioRecorder.stopCapture()
        mAudioEncoder.close()
        initListener?.unInit()
        log("already stop record")
    }

    override fun onCallback(srcData: ByteArray?) {
//        log("callback every data:${mTestStart} size:${data?.size}")
//        TimeTracker.trackBegin()

        if (!mTestStart) return
        mVideoEncoder.saveVideoByte(srcData)
//        TimeTracker.trackEnd()
    }


    fun onResume() {
        mCameraSurfaceManager.onResume()
    }

    fun onStop() {
        mCameraSurfaceManager.onStop()
    }

    override fun surfaceChanged(holder: SurfaceHolder?, format: Int, width: Int, height: Int) {
        log(
            "surfaceChanged width:${cameraSurfaceView.cameraUtil.cameraWidth} " +
                    "height:${cameraSurfaceView.cameraUtil.cameraHeight}"
        )
        mVideoEncoder.setDimensions(
            cameraSurfaceView.cameraUtil.cameraWidth,
            cameraSurfaceView.cameraUtil.cameraHeight
        )
        mVideoEncoder.setDegrees(cameraSurfaceView.cameraUtil.orientation)
    }

    override fun surfaceDestroyed(holder: SurfaceHolder?) {

    }

    override fun surfaceCreated(holder: SurfaceHolder?) {

    }

    // 录像保存到本地，直播推流
    enum class CaptureMode{
        RECORD,
        LIVE
    }
}
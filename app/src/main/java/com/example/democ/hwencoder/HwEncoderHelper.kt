package com.example.democ.hwencoder

import android.view.SurfaceHolder
import com.example.camera.listener.CameraYUVDataListener
import com.example.camera.manager.CameraSurfaceManager
import com.example.camera.manager.CameraSurfaceView
import com.example.democ.audio.*
import com.libyuv.util.YuvUtil
import java.lang.IllegalStateException

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
    private val mAudioRecorder by lazy { CustomAudioCapturer() }
    private val player by lazy { CustomAudioTrack() }
    private val mAudioEncoder by lazy { AudioEncoder() }
    private val decoder by lazy { AudioDecoder() }
    private val mVideoEncoder by lazy { VideoEncoder() }
    private lateinit var mCameraSurfaceManager:CameraSurfaceManager


    fun init() {
        if (mInited) return
        mCameraSurfaceManager = CameraSurfaceManager(cameraSurfaceView)
        cameraSurfaceView.setCameraYUVDataListener(this)
        cameraSurfaceView.setmHolderCall(this)
        mInited = true
    }

    fun startRecord() {
        if (!mInited) throw IllegalStateException("you must call init() before startRecord()")
        if (mTestStart) return
        val start = System.nanoTime()
        mTestStart = true
        MuxerManager.getInstance().init()
        mAudioRecorder.mListener = object : ByteBufferListener {
            override fun listener(buffer: ByteArray) {
                mAudioEncoder.encode(buffer, (System.nanoTime() - start) / 1000)
            }
        }
        mAudioRecorder.startCapture()
        mAudioEncoder.open()
        mAudioEncoder.startRunTask()
        mVideoEncoder.startEncode()
        log("already start record")
    }

    fun stopRecord() {
        mTestStart = false
        mAudioEncoder.close()
        mAudioRecorder.stopCapture()
        mVideoEncoder.stopEncode()
        MuxerManager.getInstance().stop()
        log("already stop record")
    }

    override fun onCallback(data: ByteArray?) {
//        log("callback every data:${mTestStart} size:${data?.size}")
        if (!mTestStart) return
        mVideoEncoder.saveVideoByte(data)
    }


    fun onResume() {
        mCameraSurfaceManager.onResume()
    }

    fun onStop() {
        mCameraSurfaceManager.onStop()
    }

    override fun surfaceChanged(holder: SurfaceHolder?, format: Int, width: Int, height: Int) {
        log("surfaceChanged width:${cameraSurfaceView.cameraUtil.cameraWidth} " +
                    "height:${cameraSurfaceView.cameraUtil.cameraHeight}")
        mVideoEncoder.setDimensions(
            cameraSurfaceView.cameraUtil.cameraWidth,
            cameraSurfaceView.cameraUtil.cameraHeight
        )
    }

    override fun surfaceDestroyed(holder: SurfaceHolder?) {

    }

    override fun surfaceCreated(holder: SurfaceHolder?) {

    }
}
package com.example.democ.hwencoder

import android.media.MediaCodec
import android.media.MediaFormat
import android.view.SurfaceHolder
import com.example.camera.listener.CameraYUVDataListener
import com.example.camera.manager.CameraSurfaceManager
import com.example.camera.manager.CameraSurfaceView
import com.example.democ.audio.*
import com.example.democ.interfaces.OutputEncodedDataListener
import java.lang.IllegalStateException
import java.nio.ByteBuffer

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
    private lateinit var mCameraSurfaceManager: CameraSurfaceManager


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
        mTestStart = true
        MuxerManager.getInstance().init()
        mAudioRecorder.mListener = object : ByteBufferListener {
            override fun listener(buffer: ByteArray) {
                mAudioEncoder.encode(buffer)
            }
        }
        val listener = object : OutputEncodedDataListener {
            override fun outputData(
                trackId: Int,
                byteBuffer: ByteBuffer,
                bufferInfo: MediaCodec.BufferInfo
            ) {
                MuxerManager.getInstance()
                    .writeSampleData(trackId, byteBuffer, bufferInfo);
            }

            override fun outputFormatChanged(mediaFormat: MediaFormat) {
                MuxerManager.getInstance().addTrack(mediaFormat)
                MuxerManager.getInstance().start()
            }
        }
        mAudioEncoder.setOutputListener(listener)
        mVideoEncoder.setOutputListener(listener)
        mAudioRecorder.startCapture()
        mAudioEncoder.startRunTask()
        mVideoEncoder.startEncode()
        log("already start record")
    }

    fun stopRecord() {
        mTestStart = false

        mVideoEncoder.stopEncode()
        mAudioRecorder.stopCapture()
        mAudioEncoder.close()
        MuxerManager.getInstance().stop()
        log("already stop record")
    }

    override fun onCallback(srcData: ByteArray?) {
//        log("callback every data:${mTestStart} size:${data?.size}")
//        TimeTracker.trackBegin()

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
}
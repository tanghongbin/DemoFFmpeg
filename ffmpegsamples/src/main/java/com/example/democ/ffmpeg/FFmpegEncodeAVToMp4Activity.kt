package com.example.democ.ffmpeg

import android.hardware.Camera
import android.os.Bundle
import android.view.SurfaceHolder
import androidx.appcompat.app.AppCompatActivity
import com.example.camera.listener.CameraYUVDataListener
import com.example.camera.manager.CameraSurfaceManager
import com.example.democ.R
import com.example.democ.audio.log
import com.example.democ.render.FFmpegRender
import com.example.democ.utils.CameraAsyncDataHelper
import com.example.democ.utils.SpUtils
import kotlinx.android.synthetic.main.activity_f_fmpeg_encode_a_v_to_mp4.*

/**
 *
 * author : tanghongbin
 *
 * date   : 2020/11/30 14:39
 *
 * desc   : 将麦克风，摄像机采集的数据编码成aac,h264再合并输出成mp4文件
 *
 **/
class FFmpegEncodeAVToMp4Activity : AppCompatActivity(), SurfaceHolder.Callback,
    CameraYUVDataListener {

    lateinit var mCameraSurfaceManager: CameraSurfaceManager
    lateinit var mFFmpegRender: FFmpegRender
    private val cameraAsyncHelper by lazy { CameraAsyncDataHelper() }
    private var startTime = 0L
    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        setContentView(R.layout.activity_f_fmpeg_encode_a_v_to_mp4)
        mCameraSurfaceManager = CameraSurfaceManager(mAvSurface)
//        mCameraSurfaceManager.mCameraUtil.currentCameraType = Camera.CameraInfo.CAMERA_FACING_FRONT
        mFFmpegRender = FFmpegRender()
        mAvSurface.setCameraYUVDataListener(this)
        mStartRecordMp4.setOnClickListener {
            mFFmpegRender.native_startrecordmp4()
            startTime = System.currentTimeMillis()
        }
        mStopRecordMp4.setOnClickListener {
            mFFmpegRender.native_stoprecordmp4()
            log("录制结束，功耗时:${System.currentTimeMillis() - startTime}")
        }

        mAvSurface.setmHolderCall(this)
        cameraAsyncHelper.mListener = CameraYUVDataListener { data -> realEncode(data) }
    }

    override fun onResume() {
        super.onResume()
        mCameraSurfaceManager.onResume()
    }

    override fun onStop() {
        super.onStop()
        mCameraSurfaceManager.onStop()
    }

    override fun onDestroy() {
        super.onDestroy()
        cameraAsyncHelper.onDestroy()
    }

    override fun surfaceChanged(holder: SurfaceHolder?, format: Int, width: Int, height: Int) {
        mFFmpegRender.native_encodeavmuxer_OnSurfaceChanged(mAvSurface.cameraUtil.cameraWidth,
            mAvSurface.cameraUtil.cameraHeight)
    }

    override fun surfaceDestroyed(holder: SurfaceHolder?) {
        log("java layer surfaceDestroyed")
        val result = mFFmpegRender.native_unInitRecordMp4()
        log("打印播放地址:${result}")
        SpUtils.putString("url",result)
    }

    override fun surfaceCreated(holder: SurfaceHolder?) {
        mFFmpegRender.native_encodeavmuxer_OnSurfaceCreated()
    }

    override fun onCallback(nv21: ByteArray?) {
        cameraAsyncHelper.addByteData(nv21)
    }

    private fun realEncode(nv21: ByteArray?) {
//        log("java layer onCallback")

        if (nv21 == null) return
//        val windowWidth = mAvSurface.cameraUtil.cameraWidth
//        val windowHeight = mAvSurface.cameraUtil.cameraHeight
//        val scaleHeight = mAvSurface.cameraUtil.cameraWidth
//        val scaleWidth = mAvSurface.cameraUtil.cameraHeight
//        val mOrientation = mAvSurface.cameraUtil.orientation
//        val bytesSize = nv21?.size ?: 0
//
//        val start = System.currentTimeMillis()
//
//        val finalResult = JavaYuvConvertHelper.convert(
//            nv21,
//            windowWidth,
//            windowHeight,
//            mOrientation,
//            mOrientation == 270
//        )
//        val middle2 = System.currentTimeMillis()
        //        log("nv21 转 i420 耗时:${middle - start}   旋转压缩耗时:${middle2 - middle}" +
        //                "      width:${windowWidth} height:${windowHeight} orientation:${mOrientation}  数组大小:${bytesSize}")
        mFFmpegRender.native_encodeavmuxer_encodeFrame(nv21)
    }


}

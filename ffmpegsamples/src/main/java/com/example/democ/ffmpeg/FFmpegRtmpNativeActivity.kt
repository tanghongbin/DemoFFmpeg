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
import com.example.democ.utils.JavaYuvConvertHelper
import com.libyuv.LibyuvUtil
import kotlinx.android.synthetic.main.activity_live_rtmp.*

/**
 *
 * author : tanghongbin
 *
 * date   : 2020/11/30 14:39
 *
 * desc   : 将麦克风，摄像机采集的数据编码成aac,h264再合并输出成mp4文件
 *
 **/
class FFmpegRtmpNativeActivity : AppCompatActivity(), SurfaceHolder.Callback,
    CameraYUVDataListener {

    lateinit var mCameraSurfaceManager: CameraSurfaceManager
    lateinit var mFFmpegRender: FFmpegRender
    private val cameraAsyncHelper by lazy { CameraAsyncDataHelper() }

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        setContentView(R.layout.activity_live_rtmp)
        mCameraSurfaceManager = CameraSurfaceManager(mLiveSurface)
        mCameraSurfaceManager.mCameraUtil.currentCameraType =
            Camera.CameraInfo.CAMERA_FACING_FRONT
        mFFmpegRender = FFmpegRender()
        mLiveSurface.setCameraYUVDataListener(this)
        mStartPushRtmp.setOnClickListener {
            mFFmpegRender.native_live_startpush()
        }
        mStopPushRtmp.setOnClickListener {
            mFFmpegRender.native_live_stoppush()
        }
        cameraAsyncHelper.mListener = CameraYUVDataListener { data -> realEncode(data) }

        mLiveSurface.setmHolderCall(this)
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
        mFFmpegRender.native_live__OnSurfaceChanged(mLiveSurface.cameraUtil.cameraWidth,
            mLiveSurface.cameraUtil.cameraHeight)
    }

    override fun surfaceDestroyed(holder: SurfaceHolder?) {
        log("java layer surfaceDestroyed")
        val result = mFFmpegRender.native_live_onDestroy()
        log("打印播放地址:${result}")
//        SpUtils.putString("url",result)
    }

    override fun surfaceCreated(holder: SurfaceHolder?) {
        mFFmpegRender.native_live_OnSurfaceCreated()
    }

    override fun onCallback(srcData: ByteArray?) {
        cameraAsyncHelper.addByteData(srcData)
    }

    private fun realEncode(nv21:ByteArray?) {
//        log("real encode")
        val windowWidth = mLiveSurface.cameraUtil.cameraWidth
        val windowHeight = mLiveSurface.cameraUtil.cameraHeight
        val scaleHeight = mLiveSurface.cameraUtil.cameraWidth
        val scaleWidth = mLiveSurface.cameraUtil.cameraHeight
        val mOrientation = mLiveSurface.cameraUtil.orientation
        val bytesSize = nv21?.size ?: 0

        val start = System.currentTimeMillis()

        val finalResult = JavaYuvConvertHelper.convert(
            nv21,
            windowWidth,
            windowHeight,
            mOrientation,
            mOrientation == 270
        )
        val middle2 = System.currentTimeMillis()
//                log("nv21 转 i420 耗时:${middle - start}   旋转压缩耗时:${middle2 - middle}" +
        //                "      width:${cameraWidth} height:${windowHeight} orientation:${mOrientation}  数组大小:${bytesSize}")
//        log("判断bytearray是否为空:${finalResult == null} 大小:${finalResult.size}")
        mFFmpegRender.native_live_encodeFrame(finalResult)
    }


}

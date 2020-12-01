package com.example.democ.ffmpeg

import androidx.appcompat.app.AppCompatActivity
import android.os.Bundle
import android.view.SurfaceHolder
import com.example.camera.listener.CameraYUVDataListener
import com.example.camera.manager.CameraSurfaceManager
import com.example.camera.util.SPUtil
import com.example.democ.R
import com.example.democ.audio.log
import com.example.democ.render.FFmpegRender
import com.example.democ.utils.SpUtils
import com.libyuv.util.YuvUtil
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
    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        setContentView(R.layout.activity_f_fmpeg_encode_a_v_to_mp4)
        mCameraSurfaceManager = CameraSurfaceManager(mAvSurface)
        mFFmpegRender = FFmpegRender()
        mAvSurface.setCameraYUVDataListener(this)
        mStartRecordMp4.setOnClickListener {
            mFFmpegRender.native_startrecordmp4()
        }
        mStopRecordMp4.setOnClickListener {
            mFFmpegRender.native_stoprecordmp4()
        }

        mAvSurface.setmHolderCall(this)
    }

    override fun onResume() {
        super.onResume()
        mCameraSurfaceManager.onResume()
    }

    override fun onStop() {
        super.onStop()
        mCameraSurfaceManager.onStop()
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

    override fun onCallback(srcData: ByteArray?) {
        log("java layer onCallback")
        val start = System.currentTimeMillis()
        val windowWidth = mAvSurface.cameraUtil.cameraWidth
        val windowHeight = mAvSurface.cameraUtil.cameraHeight
        val scaleHeight = mAvSurface.cameraUtil.cameraWidth
        val scaleWidth = mAvSurface.cameraUtil.cameraHeight
        val mOrientation = mAvSurface.cameraUtil.morientation
        val bytes = ByteArray(windowHeight * windowWidth * 3 / 2)
        YuvUtil.yuvCompress(
            srcData,
            windowWidth,
            windowHeight,
            bytes,
            scaleHeight,
            scaleWidth,
            3,
            mOrientation,
            mOrientation == 270
        )
        val middle = System.currentTimeMillis()
        mFFmpegRender.native_encodeavmuxer_encodeFrame(bytes)
        log("yuv 旋转耗时:${middle - start}  编码时间:${System.currentTimeMillis() - middle}")
    }


}

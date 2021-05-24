package com.example.ijkplayer_demo

import android.opengl.GLSurfaceView
import androidx.appcompat.app.AppCompatActivity
import android.os.Bundle
import android.os.Environment
import android.util.Log
import android.view.Surface
import android.view.SurfaceHolder
import android.view.SurfaceView
import com.example.common_base.utils.changeScreenSize
import com.example.common_base.utils.log
import com.example.common_base.utils.requestCustomPermissions
import kotlinx.android.synthetic.main.activity_ijk_player.*
import tv.danmaku.ijk.media.player.IjkMediaPlayer

class FFmpegIjkPlayerActivity : AppCompatActivity(), SurfaceHolder.Callback {

    private val mMediaPlayer by lazy { IjkMediaPlayer() }
    private val mUrl by lazy { FVL_MY_LOGO }

    companion object{
        val prefix = Environment.getExternalStorageDirectory().absolutePath
        val FILD_VIDEO_DIR = "/ffmpegtest/videos"
        val FVL_MY_LOGO = prefix + "${FILD_VIDEO_DIR}/my_logo.flv"
        val MP4_PLAY_PATH = prefix + "${FILD_VIDEO_DIR}/video.mp4"
        val MP4_PLAY_BIG_PATH = prefix + "${FILD_VIDEO_DIR}/blackanimal.mp4"
    }

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        setContentView(R.layout.activity_ijk_player)
        requestCustomPermissions {
            mIjkSurface.holder.addCallback(this)
        }
    }

    private fun init() {
        log("has init success")
        mMediaPlayer.setOnErrorListener { iMediaPlayer, i, i2 ->
            logd("errorCode:${i} errorCode2:${i2}")
            false
        }
        mMediaPlayer.setOnVideoSizeChangedListener { p0, width, height, p3, p4 ->
            mIjkSurface!!.changeScreenSize(width,height)
        }
        mMediaPlayer.dataSource = mUrl
        mMediaPlayer.setOnPreparedListener {
            mMediaPlayer.start()
        }
        mMediaPlayer.prepareAsync()
    }

    override fun onDestroy() {
        mMediaPlayer.release()
        super.onDestroy()
    }

    override fun surfaceChanged(holder: SurfaceHolder?, format: Int, width: Int, height: Int) {

    }

    override fun surfaceDestroyed(holder: SurfaceHolder?) {
    }

    override fun surfaceCreated(holder: SurfaceHolder?) {
        mMediaPlayer.setSurface(holder!!.surface)
        init()
    }


}

fun logd(str:String){
    Log.d("DemoC",str)
}
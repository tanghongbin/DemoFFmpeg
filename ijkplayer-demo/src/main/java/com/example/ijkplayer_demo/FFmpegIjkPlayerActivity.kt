package com.example.ijkplayer_demo

import androidx.appcompat.app.AppCompatActivity
import android.os.Bundle
import android.os.Environment
import android.util.Log
import android.view.Surface
import android.view.SurfaceHolder
import android.view.SurfaceView
import com.example.common_base.utils.changeScreenSize
import tv.danmaku.ijk.media.player.IMediaPlayer
import tv.danmaku.ijk.media.player.IjkMediaPlayer

class FFmpegIjkPlayerActivity : AppCompatActivity(), SurfaceHolder.Callback {

    private lateinit var mSurfaceView: SurfaceView
    private val mMediaPlayer by lazy { IjkMediaPlayer() }

    companion object{
        val FILD_VIDEO_DIR = "/ffmpegtest/videos"
        val MP4_PLAY_PATH = Environment.getExternalStorageDirectory().absolutePath + "${FILD_VIDEO_DIR}/video.mp4"
        val MP4_PLAY_BIG_PATH = Environment.getExternalStorageDirectory().absolutePath + "${FILD_VIDEO_DIR}/blackanimal.mp4"
    }

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        setContentView(R.layout.activity_ijk_player)
        IjkMediaPlayer.loadLibrariesOnce(null)
        IjkMediaPlayer.native_profileBegin("libijkplayer.so")
        mSurfaceView = findViewById<SurfaceView>(R.id.mSurfaceView)
        mSurfaceView.holder.addCallback(this)
    }

    private fun init(surface: Surface) {
        mMediaPlayer.setSurface(surface)
        mMediaPlayer.setOnErrorListener { iMediaPlayer, i, i2 ->
            logd("errorCode:${i} errorCode2:${i2}")
            true
        }
        mMediaPlayer.setOnVideoSizeChangedListener { p0, width, height, p3, p4 ->
            mSurfaceView.changeScreenSize(width,height)
        }
        mMediaPlayer.dataSource = MP4_PLAY_BIG_PATH
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
        init(holder!!.surface!!)
    }


}

fun logd(str:String){
    Log.d("DemoC",str)
}
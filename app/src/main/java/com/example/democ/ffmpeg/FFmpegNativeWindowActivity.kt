package com.example.democ.ffmpeg

import androidx.appcompat.app.AppCompatActivity
import android.os.Bundle
import android.view.SurfaceHolder
import com.example.democ.R
import com.example.democ.audio.MuxerManager
import com.example.democ.render.FFmpegRender
import com.example.democ.utils.SpUtils
import kotlinx.android.synthetic.main.activity_f_fmpeg_native_window.*

class FFmpegNativeWindowActivity : AppCompatActivity(), SurfaceHolder.Callback {

    lateinit var render:FFmpegRender
    private val mPlayPath:String by lazy {
       val result =  if (intent.getStringExtra("path").isNullOrBlank()) MuxerManager.MP4_PLAY_PATH
        else intent.getStringExtra("path")
        result
//       val result =  SpUtils.getString("url")
//        result ?: ""
    }
    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        setContentView(R.layout.activity_f_fmpeg_native_window)
        mSurface.holder.addCallback(this)
        render = FFmpegRender()
    }

    override fun surfaceChanged(holder: SurfaceHolder?, format: Int, width: Int, height: Int) {

    }

    override fun surfaceDestroyed(holder: SurfaceHolder?) {
        render.native_unInit()
    }

    override fun surfaceCreated(holder: SurfaceHolder?) {
//        val url = MuxerManager.ENCODE_VIDEO_PATH

        // 1-音频，2-视频
        render.playMP4(mPlayPath, holder?.surface)
    }
}

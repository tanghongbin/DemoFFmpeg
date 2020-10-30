package com.example.democ.ffmpeg

import androidx.appcompat.app.AppCompatActivity
import android.os.Bundle
import android.view.SurfaceHolder
import com.example.democ.R
import com.example.democ.audio.MuxerManager
import com.example.democ.render.FFmpegRender
import kotlinx.android.synthetic.main.activity_f_fmpeg_native_window.*

class FFmpegNativeWindowActivity : AppCompatActivity(), SurfaceHolder.Callback {

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        setContentView(R.layout.activity_f_fmpeg_native_window)
        mSurface.holder.addCallback(this)
    }

    override fun surfaceChanged(holder: SurfaceHolder?, format: Int, width: Int, height: Int) {

    }

    override fun surfaceDestroyed(holder: SurfaceHolder?) {

    }

    override fun surfaceCreated(holder: SurfaceHolder?) {
//        val url = MuxerManager.MP4_PLAY_BIG_PATH
        val url = MuxerManager.ENCODE_VIDEO_PATH

        // 1-音频，2-视频
        val render = FFmpegRender()
        render.playMP4(url, holder?.surface, 2)
//        render.playMP4(url, holder?.surface, 1)
    }
}

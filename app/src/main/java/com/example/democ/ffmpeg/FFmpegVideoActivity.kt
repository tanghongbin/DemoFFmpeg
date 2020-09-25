package com.example.democ.ffmpeg

import androidx.appcompat.app.AppCompatActivity
import android.os.Bundle
import android.view.SurfaceHolder
import android.view.WindowManager
import com.example.democ.R
import com.example.democ.audio.MuxerManager.Companion.MP4_PLAY_BIG_PATH
import com.example.democ.audio.MuxerManager.Companion.MP4_PLAY_PATH
import com.example.democ.opengles.NativeRender
import com.example.democ.requestPermissions
import com.tbruyelle.rxpermissions2.RxPermissions
import kotlinx.android.synthetic.main.activity_f_fmpeg_video.*

class FFmpegVideoActivity : AppCompatActivity(), SurfaceHolder.Callback {

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        setContentView(R.layout.activity_f_fmpeg_video)
        mSurface.holder.addCallback(this)
        keepScreenOn()
    }




    override fun surfaceChanged(holder: SurfaceHolder?, format: Int, width: Int, height: Int) {

    }

    override fun surfaceDestroyed(holder: SurfaceHolder?) {

    }

    override fun surfaceCreated(holder: SurfaceHolder?) {

        val url = MP4_PLAY_PATH
//        val url = MP4_PLAY_BIG_PATH
        // 1-音频，2-视频
        Thread{
            holder?.let { NativeRender().playMP4(url,holder.surface,1) }
        }.start()
        Thread{
            holder?.let { NativeRender().playMP4(url,holder.surface,2) }
        }.start()

    }
}

/**
@author 汤洪斌
@time 2019/3/19 0019 9:10
@version 1.0
@describe 保持屏幕敞亮
 */
fun AppCompatActivity.keepScreenOn() {
    window.addFlags(WindowManager.LayoutParams.FLAG_KEEP_SCREEN_ON)
}
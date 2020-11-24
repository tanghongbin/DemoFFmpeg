package com.example.democ.ffmpeg

import android.app.ActivityManager
import android.content.Context
import android.content.pm.ConfigurationInfo
import android.opengl.GLSurfaceView
import android.os.Bundle
import androidx.appcompat.app.AppCompatActivity
import com.example.democ.R
import com.example.democ.audio.log
import com.example.democ.render.FFmpegOpenGLRender
import kotlinx.android.synthetic.main.activity_f_fmpeg_open_g_l.*

class FFmpegOpenGLActivity : AppCompatActivity() {

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        setContentView(R.layout.activity_f_fmpeg_open_g_l)
        init()
    }


    private fun init() {
//        mOpenGlEs.init(FFmpegOpenGLRender())
    }
}

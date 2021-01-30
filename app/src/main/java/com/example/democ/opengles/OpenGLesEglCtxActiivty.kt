package com.example.democ.opengles

import android.content.Context
import android.opengl.GLSurfaceView.RENDERMODE_CONTINUOUSLY
import androidx.appcompat.app.AppCompatActivity
import android.os.Bundle
import android.os.UserManager
import android.util.Log
import com.example.democ.R
import com.example.democ.audio.MuxerManager
import kotlinx.android.synthetic.main.activity_native_open_g_l_e_s_actiivty.*

class OpenGLesEglCtxActiivty : AppCompatActivity() {

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        setContentView(R.layout.empty_layout)
        val render = NativeRender()
        render.egl_init_2()
    }
}

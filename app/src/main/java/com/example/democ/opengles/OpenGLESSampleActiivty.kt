package com.example.democ.opengles

import android.content.Context
import android.opengl.GLSurfaceView.RENDERMODE_CONTINUOUSLY
import androidx.appcompat.app.AppCompatActivity
import android.os.Bundle
import android.os.UserManager
import android.util.Log
import com.example.democ.R
import com.example.democ.audio.MuxerManager
import com.example.democ.audio.log
import com.example.democ.utils.Constants
import kotlinx.android.synthetic.main.activity_native_open_g_l_e_s_actiivty.*

class OpenGLESSampleActiivty : AppCompatActivity() {

    val render by lazy { NativeRender() }

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        setContentView(R.layout.activity_native_open_g_l_e_s_actiivty)
        mSurface.init(this,render)
        log("打印测试地址:${Constants.TEST_JPG}")
    }

    override fun onDestroy() {
        render.native_OnUnInit()
        super.onDestroy()
    }

}

package com.example.democ.ffmpeg

import android.app.ActivityManager
import android.content.Context
import android.content.pm.ConfigurationInfo
import androidx.appcompat.app.AppCompatActivity
import android.os.Bundle
import com.example.democ.R
import com.example.democ.audio.log
import com.example.democ.opengles.NativeRender
import com.example.democ.render.FFmpegOpenGLRender
import kotlinx.android.synthetic.main.activity_f_fmpeg_open_g_l.*

class FFmpegOpenGLActivity : AppCompatActivity() {

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        setContentView(R.layout.activity_f_fmpeg_open_g_l)
        init()
    }

    private fun init() {
        val activityManager: ActivityManager =
            getSystemService(Context.ACTIVITY_SERVICE) as ActivityManager
        val configurationInfo: ConfigurationInfo = activityManager.deviceConfigurationInfo
        val supportsEs2: Boolean = configurationInfo.reqGlEsVersion >= 0x20000
        log("当前是否支持:${supportsEs2}")

        if (supportsEs2) { // 请求一个OpenGL ES 2.0兼容的上下文
            mOpenGlEs.setEGLContextClientVersion(2)
            // 设置我们的Demo渲染器，定义在后面讲
            mOpenGlEs.setRenderer(FFmpegOpenGLRender())
        } else { // 如果您想同时支持ES 1.0和2.0的话，这里您可以创建兼容OpenGL ES 1.0的渲染器
            return
        }
    }
}

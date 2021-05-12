package com.example.democ.opengles

import androidx.appcompat.app.AppCompatActivity
import android.os.Bundle
import com.example.democ.R
import com.example.democ.audio.log
import com.example.democ.utils.Constants
import kotlinx.android.synthetic.main.activity_native_open_g_l_e_s_actiivty.*

class OpenGLESSampleActiivty : AppCompatActivity() {

    val render by lazy { NativeRender(intent.getIntExtra("type",1)) }

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        setContentView(R.layout.activity_native_open_g_l_e_s_actiivty)
        mSurface.init(this,render)
        log("打印测试地址:${Constants.TEST_JPG}")
        mPlus.setOnClickListener {
            render.native_changeZValue(2,0.5f)
        }
        mReduce.setOnClickListener {
            render.native_changeZValue(1,0.5f)
        }
    }

    override fun onDestroy() {
        render.native_OnUnInit()
        super.onDestroy()
    }

}

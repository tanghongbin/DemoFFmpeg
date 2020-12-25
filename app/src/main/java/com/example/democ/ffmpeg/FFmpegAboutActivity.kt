package com.example.democ.ffmpeg

import androidx.appcompat.app.AppCompatActivity
import android.os.Bundle
import android.view.View
import com.example.democ.R
import com.example.democ.audio.log
import com.example.democ.render.FFmpegRender
import kotlinx.android.synthetic.main.activity_f_fmpeg_encode_audio.*
import kotlinx.android.synthetic.main.activity_ffmpeg_about.*
import java.util.regex.Pattern

class FFmpegAboutActivity : AppCompatActivity(),View.OnClickListener {

    lateinit var render: FFmpegRender
    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        setContentView(R.layout.activity_ffmpeg_about)
        render = FFmpegRender()
        val str = render.native_getSimpleInfo()
        mFFmpegText.text = str
        log("字符串是否包含:${str.contains("rtmp")}")
    }

    override fun onDestroy() {
        render.native_unInit()
        super.onDestroy()
    }


    override fun onClick(v: View?) {
        when(v?.id){
        }
    }

}

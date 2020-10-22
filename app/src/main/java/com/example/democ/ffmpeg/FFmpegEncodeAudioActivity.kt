package com.example.democ.ffmpeg

import androidx.appcompat.app.AppCompatActivity
import android.os.Bundle
import android.view.View
import com.example.democ.R
import com.example.democ.audio.log
import com.example.democ.render.FFmpegRender
import kotlinx.android.synthetic.main.activity_f_fmpeg_encode_audio.*
import java.util.regex.Pattern

class FFmpegEncodeAudioActivity : AppCompatActivity(),View.OnClickListener {

    lateinit var render: FFmpegRender
    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        setContentView(R.layout.activity_f_fmpeg_encode_audio)
        button.setOnClickListener(this)
        button2.setOnClickListener(this)
        button3.setOnClickListener(this)
        button4.setOnClickListener(this)
        render = FFmpegRender()
    }

    override fun onDestroy() {
        render.native_unInit()
        super.onDestroy()
    }


    override fun onClick(v: View?) {
        when(v?.id){
            R.id.button -> {
                Thread {
                    render.native_audioTest(1)
                }.start()
            }
            R.id.button2 -> {
                render.native_audioTest(2)
            }
            R.id.button3 -> {
                Thread {
                    render.native_audioTest(3)
                }.start()
            }
            R.id.button4 -> {
                render.native_audioTest(4)
            }
        }
    }

}

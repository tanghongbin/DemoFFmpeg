package com.example.customplayer.activity.ffmpeg

import androidx.appcompat.app.AppCompatActivity
import android.os.Bundle
import com.example.customplayer.R
import com.example.customplayer.player.CustomMediaController
import kotlinx.android.synthetic.main.activity_f_fmpeg_muxer.*

class FFmpegMuxerActivity : AppCompatActivity() {
    private val mMuxer by lazy { CustomMediaController(2) }
    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        setContentView(R.layout.activity_f_fmpeg_muxer)
        muxerButton.setOnClickListener {
            mMuxer.startTestEncode()
        }
    }
}
package com.example.ijkplayer_demo

import android.content.Intent
import androidx.appcompat.app.AppCompatActivity
import android.os.Bundle
import com.example.common_base.utils.createIntent
import kotlinx.android.synthetic.main.activity_ijk_splash.*
import tv.danmaku.ijk.media.player.IjkMediaPlayer

class IjkSplashActivity : AppCompatActivity() {
    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        setContentView(R.layout.activity_ijk_splash)
        IjkMediaPlayer.loadLibrariesOnce(null)
        IjkMediaPlayer.native_profileBegin("libijkplayer.so")

        button.setOnClickListener {
            startActivity(createIntent(FFmpegIjkPlayerActivity::class.java))
        }
    }
}
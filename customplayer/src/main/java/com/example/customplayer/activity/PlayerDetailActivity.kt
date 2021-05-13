package com.example.customplayer.activity

import androidx.appcompat.app.AppCompatActivity
import android.os.Bundle
import com.example.customplayer.R
import com.example.customplayer.player.CustomPlayer
import kotlinx.android.synthetic.main.activity_player_detail.*

class PlayerDetailActivity : AppCompatActivity() {
    private val mUrl by lazy { intent.getStringExtra("url") }
    private val mPlayer by lazy { CustomPlayer() }
    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        setContentView(R.layout.activity_player_detail)
        mGLSurface.init(mPlayer)
    }

    override fun onDestroy() {
        mPlayer.native_OnDestroy()
        super.onDestroy()
    }
}
package com.example.customplayer.activity.ffmpeg

import android.os.Build
import android.os.Bundle
import androidx.annotation.RequiresApi
import androidx.appcompat.app.AppCompatActivity
import com.example.customplayer.R
import com.example.customplayer.player.CustomMediaController

/***
 * webrtc 实时通信
 */
class WebrtcActivity : AppCompatActivity(){

    private val mPlayer by lazy { CustomMediaController() }
    @RequiresApi(Build.VERSION_CODES.O)
    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        setContentView(R.layout.activity_webrtc)
    }


    override fun onDestroy() {
//        log("打印指针：${mPlayer.mNativePlayer}")
        mPlayer.native_OnDestroy()
        super.onDestroy()
    }

}
package com.example.customplayer.activity.ffmpeg

import android.content.pm.ActivityInfo
import android.media.MediaPlayer
import android.os.Build
import androidx.appcompat.app.AppCompatActivity
import android.os.Bundle
import android.view.SurfaceHolder
import android.widget.SeekBar
import androidx.annotation.RequiresApi
import com.example.avutils.audio.decoder.AudioDecodeManager
import com.example.avutils.video.decoder.OutputFormatChangeListener
import com.example.avutils.video.decoder.VideoDecodeManager
import com.example.common_base.utils.Constants
import com.example.common_base.utils.Constants.TIME_UNIT_SEC
import com.example.common_base.utils.Constants.TIME_UNIT_US
import com.example.common_base.utils.changeScreenSize
import com.example.common_base.utils.log
import com.example.customplayer.R
import com.example.customplayer.interfaces.OnCompleteListener
import com.example.customplayer.interfaces.OnErrorListener
import com.example.customplayer.interfaces.OnPreparedListener
import com.example.customplayer.player.CustomPlayer
import kotlinx.android.synthetic.main.activity_player_detail.*
import java.util.concurrent.TimeUnit
import kotlin.random.Random

/***
 * 直接用mediaplayer播放
 */
class PlayerDecodeFFmpegActivity : AppCompatActivity(){
    private val mUrl by lazy {
//        intent.getStringExtra("url")
        Constants.MP4_PLAY_PATH
    }
    private var isPlaying = true
    private val mPlayer by lazy { CustomPlayer() }
    @RequiresApi(Build.VERSION_CODES.O)
    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        setContentView(R.layout.activity_player_detail)
        mGLSurface.init(mPlayer)
        button.setOnClickListener {
            mPlayer.nativeGetInfo()
            var oreration = requestedOrientation
            if (oreration == ActivityInfo.SCREEN_ORIENTATION_PORTRAIT ||
                    oreration == ActivityInfo.SCREEN_ORIENTATION_UNSPECIFIED) {
                oreration = ActivityInfo.SCREEN_ORIENTATION_LANDSCAPE
            } else {
                oreration = ActivityInfo.SCREEN_ORIENTATION_PORTRAIT
            }
            requestedOrientation = oreration
        }
        mPlayer.native_setDataUrl(mUrl)
        mPlayer.setPrepareListener(object : OnPreparedListener{
            override fun onPrepared() {
                log("i am already prepared")
                mPlayer.native_start()
                button.text = "改变文字${Random(10000).nextInt()}"
            }
        })
        mPlayer.setOnCompleteListener(object : OnCompleteListener{
            override fun onComplete() {
                log("播放完成")
            }
        })
        mPlayer.setOnErrorListener(object : OnErrorListener{
            override fun onError(code: Int, str: String) {
                log("errorCode:${code} str:${str}")
            }
        })
        mPlayer.native_prepare()
        mPlayPause.setOnClickListener {
            isPlaying = !isPlaying
            if (isPlaying){
                mPlayer.native_start()
            } else {
                mPlayer.native_stop()
            }
            mPlayPause.setImageResource(if (!isPlaying) R.mipmap.live_resume else R.mipmap.live_pause)
        }
    }

    override fun onDestroy() {
        mPlayer.native_OnDestroy()
        super.onDestroy()
    }

}
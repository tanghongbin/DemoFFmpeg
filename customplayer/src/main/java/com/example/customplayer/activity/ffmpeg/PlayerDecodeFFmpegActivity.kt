package com.example.customplayer.activity.ffmpeg

import android.content.pm.ActivityInfo
import android.os.Build
import androidx.appcompat.app.AppCompatActivity
import android.os.Bundle
import android.widget.SeekBar
import androidx.annotation.RequiresApi
import com.example.common_base.utils.log
import com.example.common_base.utils.toastSafe
import com.example.customplayer.R
import com.example.customplayer.interfaces.*
import com.example.customplayer.player.CustomMediaController
import kotlinx.android.synthetic.main.activity_player_detail.*
import kotlin.random.Random

/***
 * 直接用mediaplayer播放
 */
class PlayerDecodeFFmpegActivity : AppCompatActivity(){
    private val mUrl by lazy {
        intent.getStringExtra("url") ?: ""
//        Constants.MP4_PLAY_PATH
    }
    private var isPlaying = true
    private val mPlayer by lazy { CustomMediaController() }
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
        mPlayPause.setOnClickListener {
            isPlaying = !isPlaying
            if (isPlaying){
                mPlayer.native_start()
            } else {
                mPlayer.native_stop()
            }
            mPlayPause.setImageResource(if (!isPlaying) R.mipmap.live_resume else R.mipmap.live_pause)
        }
        mSeekbar.setOnSeekBarChangeListener(object : SeekBar.OnSeekBarChangeListener{
            override fun onProgressChanged(seekBar: SeekBar?, progress: Int, fromUser: Boolean) {
                if (fromUser) mPlayer.native_seekTo(seekBar?.progress ?: 0)
            }

            override fun onStartTrackingTouch(seekBar: SeekBar?) {
            }

            override fun onStopTrackingTouch(seekBar: SeekBar?) {

            }

        })
        setupListener()
        mPlayer.native_setDataUrl(mUrl)
        mPlayer.native_prepare()
        log("打印当前时间java:${System.currentTimeMillis()}")
    }

    private fun setupListener() {
        mPlayer.setPrepareListener(object : OnPreparedListener {
            override fun onPrepared() {
                log("i am already prepared")
                mPlayer.native_start()
                button.text = "改变文字${Random(10000).nextInt()}"
            }
        })
        mPlayer.setOnCompleteListener(object : OnCompleteListener {
            override fun onComplete() {
                log("播放完成")
            }
        })
        mPlayer.setOnErrorListener(object : OnErrorListener {
            override fun onError(code: Int, str: String) {
                log("errorCode:${code} str:${str}")
                toastSafe("播放失败 $str")
            }
        })
        mPlayer.setOnDurationListener(object : OnDurationListener{
            override fun onDuration(min: Int, max: Int) {
                mSeekbar.min = min
                mSeekbar.max = max
            }
        })
        mPlayer.setOnSeekProgressChangeListener(object : OnSeekProgressChangeListener{
            override fun onProgress(progress: Int) {
                mSeekbar.progress = progress
            }
        })
    }

    override fun onDestroy() {
//        log("打印指针：${mPlayer.mNativePlayer}")
        mPlayer.destroy()
        super.onDestroy()
    }

}
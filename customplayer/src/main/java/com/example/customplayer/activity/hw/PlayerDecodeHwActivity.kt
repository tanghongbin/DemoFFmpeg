package com.example.customplayer.activity.hw

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
import com.example.customplayer.player.CustomPlayer
import kotlinx.android.synthetic.main.activity_player_detail.*
import java.util.concurrent.TimeUnit

/***
 * 直接用mediaplayer播放
 */
class PlayerDecodeHwActivity : AppCompatActivity(), SurfaceHolder.Callback,
    OutputFormatChangeListener {
    private val mUrl by lazy { intent.getStringExtra("url") }
    private val mPlayer by lazy { CustomPlayer() }
    @RequiresApi(Build.VERSION_CODES.O)
    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        setContentView(R.layout.activity_player_detail)
        mGLSurface.holder.addCallback(this)
        mPlayer.nativeGetInfo()
        button.setOnClickListener {
            var oreration = requestedOrientation
            if (oreration == ActivityInfo.SCREEN_ORIENTATION_PORTRAIT ||
                    oreration == ActivityInfo.SCREEN_ORIENTATION_UNSPECIFIED) {
                oreration = ActivityInfo.SCREEN_ORIENTATION_LANDSCAPE
            } else {
                oreration = ActivityInfo.SCREEN_ORIENTATION_PORTRAIT
            }
            requestedOrientation = oreration
        }
        mSeekbar.min = 0
        val ass = java.lang.String()
        mSeekbar.setOnSeekBarChangeListener(object : SeekBar.OnSeekBarChangeListener{
            override fun onProgressChanged(seekBar: SeekBar?, progress: Int, fromUser: Boolean) {
            }

            override fun onStartTrackingTouch(seekBar: SeekBar?) {

            }

            override fun onStopTrackingTouch(seekBar: SeekBar?) {
                VideoDecodeManager.getInstance().seekTo(seekBar?.progress ?: 0)
            }

        })
    }

    override fun onDestroy() {
        mPlayer.native_OnDestroy()
        super.onDestroy()
    }

    override fun surfaceChanged(holder: SurfaceHolder?, format: Int, width: Int, height: Int) {

    }

    override fun surfaceDestroyed(holder: SurfaceHolder?) {
        AudioDecodeManager.getInstance().stopDecode()
        AudioDecodeManager.destroyInstance()
        VideoDecodeManager.getInstance().stopDecode()
        VideoDecodeManager.destroyInstance()
    }

    override fun surfaceCreated(holder: SurfaceHolder?) {
        VideoDecodeManager.getInstance().init(holder!!.surface,mUrl)
        VideoDecodeManager.getInstance().setOutputFormatChanged(this)
        AudioDecodeManager.getInstance().init(mUrl)
    }

    override fun outputFormatChange(width: Int?, height: Int?, duration: Long?) {
        runOnUiThread {
            mSeekbar.max = TimeUnit.MICROSECONDS.toSeconds(duration ?: 0L).toInt()
            log("打印width:${width} height:${height} duration:${mSeekbar.max}")
            mGLSurface.changeScreenSize(width ?: 0,height ?: 0)
        }
    }

    override fun currentDuration(time: Long) {
        runOnUiThread {
            mSeekbar.progress = TimeUnit.MICROSECONDS.toSeconds(time).toInt()
        }
    }
}
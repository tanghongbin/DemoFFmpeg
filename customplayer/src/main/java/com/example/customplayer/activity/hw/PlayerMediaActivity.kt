package com.example.customplayer.activity.hw

import android.content.pm.ActivityInfo
import android.media.MediaPlayer
import android.os.Build
import androidx.appcompat.app.AppCompatActivity
import android.os.Bundle
import android.view.SurfaceHolder
import android.widget.SeekBar
import androidx.annotation.RequiresApi
import com.example.avutils.video.decoder.OutputFormatChangeListener
import com.example.common_base.utils.Constants.TIME_UNIT_US
import com.example.common_base.utils.changeScreenSize
import com.example.common_base.utils.log
import com.example.customplayer.R
import com.example.customplayer.player.CustomMediaController
import kotlinx.android.synthetic.main.activity_player_detail.*
import java.util.concurrent.TimeUnit

/***
 * 直接用mediaplayer播放
 */
class PlayerMediaActivity : AppCompatActivity(), SurfaceHolder.Callback,
    OutputFormatChangeListener {
    private val mUrl by lazy { intent.getStringExtra("url") }
    private val mPlayer by lazy { CustomMediaController() }
    private val mMediaPlayer by lazy { MediaPlayer() }
    @RequiresApi(Build.VERSION_CODES.O)
    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        setContentView(R.layout.activity_player_detail)
        mGLSurface.holder.addCallback(this)
        mMediaPlayer.setDataSource(mUrl)
        mMediaPlayer.setOnPreparedListener {
            log("总时长：${it.duration / 1000}")
            mSeekbar.max = it.duration / 1000
            mMediaPlayer.start()
        }
        mMediaPlayer.setOnSeekCompleteListener {
            log("seek complelte")
        }
        mMediaPlayer.setOnErrorListener { mp, what, extra ->
            log("error code:${what} msg;${extra}")
            true
        }
        mMediaPlayer.setOnVideoSizeChangedListener { mp, width, height ->
            mGLSurface.changeScreenSize(width,height)
        }
        mMediaPlayer.setOnTimedTextListener { mp, text ->
            log("打印时间:${text.text}")
        }
        mMediaPlayer.prepareAsync()
//        mGLSurface.init(mPlayer)
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
        mSeekbar.setOnSeekBarChangeListener(object : SeekBar.OnSeekBarChangeListener{
            override fun onProgressChanged(seekBar: SeekBar?, progress: Int, fromUser: Boolean) {
            }

            override fun onStartTrackingTouch(seekBar: SeekBar?) {

            }

            override fun onStopTrackingTouch(seekBar: SeekBar?) {
//                VideoDecodeManager.getInstance().seekTo(seekBar?.progress ?: 0)
                mMediaPlayer.seekTo(TimeUnit.SECONDS.toMillis(seekBar!!.progress.toLong()).toInt())
            }

        })
    }

    override fun onDestroy() {
//        mPlayer.native_OnDestroy()
        mMediaPlayer.release()
        super.onDestroy()
    }

    override fun surfaceChanged(holder: SurfaceHolder?, format: Int, width: Int, height: Int) {

    }

    override fun surfaceDestroyed(holder: SurfaceHolder?) {
//        AudioDecodeManager.getInstance().stopDecode()
//        AudioDecodeManager.destroyInstance()
//        VideoDecodeManager.getInstance().stopDecode()
//        VideoDecodeManager.destroyInstance()
    }

    override fun surfaceCreated(holder: SurfaceHolder?) {
//        VideoDecodeManager.getInstance().init(holder!!.surface,mUrl)
//        VideoDecodeManager.getInstance().setOutputFormatChanged(this)
//        AudioDecodeManager.getInstance().init(mUrl)
        mMediaPlayer.setSurface(holder?.surface)
    }

    override fun outputFormatChange(width: Int?, height: Int?, duration: Long?) {
//        runOnUiThread {
//            mSeekbar.max = ((duration ?: 0L) / TIME_UNIT_US).toInt()
//            log("打印width:${width} height:${height} duration:${mSeekbar.max}")
//            val params = mGLSurface.layoutParams
//            params.width = width ?: 0
//            params.height = height ?: 0
//            mGLSurface.layoutParams = params
//            mGLSurface.requestLayout()
//        }
    }

    override fun currentDuration(time: Long) {
        runOnUiThread {
            mSeekbar.progress = (time / TIME_UNIT_US).toInt()
        }
    }
}
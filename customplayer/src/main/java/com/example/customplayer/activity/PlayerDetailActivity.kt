package com.example.customplayer.activity

import android.content.pm.ActivityInfo
import androidx.appcompat.app.AppCompatActivity
import android.os.Bundle
import android.view.SurfaceHolder
import android.widget.SeekBar
import com.example.avutils.audio.decoder.AudioDecodeManager
import com.example.avutils.video.decoder.OutputFormatChangeListener
import com.example.avutils.video.decoder.VideoDecodeManager
import com.example.common_base.utils.Constants.TIME_UNIT_SEC
import com.example.common_base.utils.log
import com.example.customplayer.R
import com.example.customplayer.player.CustomPlayer
import kotlinx.android.synthetic.main.activity_player_detail.*

class PlayerDetailActivity : AppCompatActivity(), SurfaceHolder.Callback,
    OutputFormatChangeListener {
    private val mUrl by lazy { intent.getStringExtra("url") }
    private val mPlayer by lazy { CustomPlayer() }
    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        setContentView(R.layout.activity_player_detail)
//        mGLSurface.init(mPlayer)
        mGLSurface.holder.addCallback(this)
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
            log("打印width:${width} height:${height} duration:${duration}")
            mSeekbar.max = (duration ?: 0L / TIME_UNIT_SEC).toInt()
            val params = mGLSurface.layoutParams
            params.width = width ?: 0
            params.height = height ?: 0
            mGLSurface.layoutParams = params
            mGLSurface.requestLayout()
        }
    }

    override fun currentDuration(time: Long) {
        runOnUiThread {
            mSeekbar.progress = (time / TIME_UNIT_SEC).toInt()
        }
    }
}
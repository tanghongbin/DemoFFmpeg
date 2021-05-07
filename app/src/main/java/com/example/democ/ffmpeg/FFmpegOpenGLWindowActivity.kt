package com.example.democ.ffmpeg

import android.os.Bundle
import android.view.SurfaceHolder
import android.widget.FrameLayout
import android.widget.SeekBar
import androidx.appcompat.app.AppCompatActivity
import com.example.democ.R
import com.example.democ.audio.log
import com.example.democ.interfaces.MsgCallback
import com.example.democ.render.FFmpegOpenGLRender
import com.example.democ.render.FFmpegRender
import com.example.democ.utils.Constants
import kotlinx.android.synthetic.main.activity_ffmpeg_opengl_window.*

class FFmpegOpenGLWindowActivity : AppCompatActivity(), SurfaceHolder.Callback {

    lateinit var render: FFmpegRender
    var isPortait = true
    private val mPlayPath:String by lazy {
//       val result =  if (intent.getStringExtra("path").isNullOrBlank()) MuxerManager.MP4_PLAY_PATH
//        else intent.getStringExtra("path")
//        result
//       val result =  SpUtils.getString("url")
//        result ?:
        Constants.MP4_PLAY_PATH
    }
    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        setContentView(R.layout.activity_ffmpeg_opengl_window)
        mSurface.holder.addCallback(this)
        render = FFmpegRender()
        render.setMsgCallback(object : MsgCallback {
            override fun callback(type: Int) {

            }

            override fun renderSizeDimensionChanged(renderWidth: Int, rederHeight: Int) {
                val params = mSurface.layoutParams as FrameLayout.LayoutParams
                params.width = renderWidth
                params.height = rederHeight
                mSurface.layoutParams = params
                mSurface.requestLayout()
            }

            override fun processRangeSetup(min: Int, max: Int) {
                mVideoSeekBar.min = min
                mVideoSeekBar.max = max
            }

            override fun processChanged(current: Int) {
                mVideoSeekBar.progress = current
            }

        })
        val metris = resources.displayMetrics
        log("log width:${metris.widthPixels} height:${metris.heightPixels} density:${metris.density}" +
                " densityDip:${metris.densityDpi}")
        mVideoSeekBar.setOnSeekBarChangeListener(object : SeekBar.OnSeekBarChangeListener{
            override fun onProgressChanged(seekBar: SeekBar?, progress: Int, fromUser: Boolean) {
                render.native_seekPosition(progress)
            }

            override fun onStartTrackingTouch(seekBar: SeekBar?) {
            }

            override fun onStopTrackingTouch(seekBar: SeekBar?) {

            }

        })
        mSurface.init(render)
    }

    override fun surfaceChanged(holder: SurfaceHolder?, format: Int, width: Int, height: Int) {

    }

    override fun surfaceDestroyed(holder: SurfaceHolder?) {
        render.native_unInit()
    }

    override fun surfaceCreated(holder: SurfaceHolder?) {
//        val url = MuxerManager.ENCODE_VIDEO_PATH

        // 1-音频，2-视频
        log("log play address :${mPlayPath}")
        render.playMP4(mPlayPath, null)
    }
}

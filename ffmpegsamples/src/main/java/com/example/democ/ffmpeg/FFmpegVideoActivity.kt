package com.example.democ.ffmpeg

import androidx.appcompat.app.AppCompatActivity
import android.os.Bundle
import android.view.SurfaceHolder
import android.widget.SeekBar
import com.example.democ.R
import com.example.democ.audio.log
import com.example.democ.interfaces.MsgCallback
import com.example.democ.opengles.NativeRender
import com.example.common_base.utils.Constants.SLIGHT_MP4_VIDEO_PATH
import com.example.democ.utils.MSG_TYPE_ONREADY
import kotlinx.android.synthetic.main.activity_f_fmpeg_video.*

class FFmpegVideoActivity : AppCompatActivity(), SurfaceHolder.Callback,MsgCallback {

    lateinit var mNativeRender: NativeRender
    var isResume = true
    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        setContentView(R.layout.activity_f_fmpeg_video)
        mNativeRender = NativeRender()
        mNativeRender.setMsgCall(this)
        mSurface.holder.addCallback(this)
        mSeekBar.setOnSeekBarChangeListener(object :SeekBar.OnSeekBarChangeListener{
            override fun onProgressChanged(seekBar: SeekBar?, progress: Int, fromUser: Boolean) {
                log("java -current progress ${seekBar?.progress}")
                mNativeRender.native_seekPosition(seekBar?.progress ?: 0)
            }

            override fun onStartTrackingTouch(seekBar: SeekBar?) {
            }

            override fun onStopTrackingTouch(seekBar: SeekBar?) {

            }
        })
        mPause.setOnClickListener {
            if (isResume){
                mNativeRender.native_pause()
            } else {
                mNativeRender.native_resume()
            }
            isResume = !isResume
            mPause.setImageResource(if (isResume) R.mipmap.live_pause else R.mipmap.live_resume)
        }
    }




    override fun surfaceChanged(holder: SurfaceHolder?, format: Int, width: Int, height: Int) {

    }

    override fun surfaceDestroyed(holder: SurfaceHolder?) {

    }

    override fun surfaceCreated(holder: SurfaceHolder?) {

//        val url = MP4_PLAY_PATH
//        val url = MP4_PLAY_BIG_PATH
        val url = SLIGHT_MP4_VIDEO_PATH

        // 1-音频，2-视频

        mNativeRender.playMP4(url,holder?.surface)


    }

    override fun callback(type: Int) {
        when(type){
            MSG_TYPE_ONREADY -> changeSize()
        }
    }

    private fun changeSize() {
        mSeekBar?.max = mNativeRender.native_getTotalDuration()?.toInt()
        log("java layer current thread:${Thread.currentThread().name} max progress:${mNativeRender.native_getTotalDuration()}")
        val params = mSurface.layoutParams
        params.width = mNativeRender.native_getVideoWidth()
        params.height = mNativeRender.native_getVideoHeight()
        mSurface.layoutParams = params
        mSurface.requestLayout()
        log("java layer chang size success ,width:${params.width}  height:${params.height}")
    }
}


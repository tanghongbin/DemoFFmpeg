package com.testthb.customplayer.activity.ffmpeg

import android.os.Build
import android.os.Bundle
import androidx.annotation.RequiresApi
import androidx.appcompat.app.AppCompatActivity
import com.testthb.avutils.audio.recoder.AudioRecorder
import com.testthb.common_base.utils.log
import com.testthb.common_base.utils.runAsyncTask
import com.testthb.customplayer.R
import com.testthb.customplayer.interfaces.OnErrorListener
import com.testthb.customplayer.player.CustomMediaRecorder
import kotlinx.android.synthetic.main.activity_capture_video.*

/**
 * 用opengles 渲染camera数据
 */
class FFmpegGLESMuxerActivity : AppCompatActivity(){
    private val mMuxer by lazy { CustomMediaRecorder() }
    private val cameraSurfaceHelper by lazy { Camera2SurfaceHelper(this,mMuxer) }
    private val mAudioRecorder by lazy { AudioRecorder() }
    private var isStart = false

    @RequiresApi(Build.VERSION_CODES.O)
    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        setContentView(R.layout.activity_capture_video)
        mRecordButton.setOnClickListener {
            if (isStart) {
                mMuxer.native_stopEncode()
            } else {
                runAsyncTask({
                    val builder = StringBuilder()
                    val buffer = StringBuffer()
                    val str = String()
                    builder.append("").toString()
                    buffer.append("").toString()
                    mAudioRecorder.startCapture()
                    mMuxer.native_setSpeed(1.0)
                    mMuxer.native_initEncode()
                })
            }
            isStart = !isStart
            mRecordButton.setImageResource(if (isStart) R.mipmap.live_pause else R.mipmap.live_resume)
        }

        mAudioRecorder.setOnAudioFrameCapturedListener { audioData, ret ->
            mMuxer.native_audioData(audioData,ret)
        }
        mGLESMuxerSurface.init(mMuxer,true)
        cameraSurfaceHelper.init()
        mMuxer.setOnErrorListener(object : OnErrorListener{
            override fun onError(code: Int, str: String) {
                log("打印错误code:${code}  msg:${str}")
            }
        })
    }

    override fun onDestroy() {
        mAudioRecorder.stopCapture()
        cameraSurfaceHelper.onDestroy()
        mMuxer.destroy()
        super.onDestroy()
    }

    override fun onPause() {
        super.onPause()
    }

}

interface A1{
    fun test1();
}

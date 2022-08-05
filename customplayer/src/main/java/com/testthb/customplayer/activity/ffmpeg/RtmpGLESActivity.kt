package com.testthb.customplayer.activity.ffmpeg

import android.os.Bundle
import androidx.appcompat.app.AppCompatActivity
import com.testthb.avutils.audio.recoder.AudioConfiguration
import com.testthb.avutils.audio.recoder.AudioRecorder
import com.testthb.common_base.utils.keepScreenOn
import com.testthb.common_base.utils.runAsyncTask
import com.testthb.customplayer.R
import com.testthb.customplayer.player.CustomMediaRecorder
import com.testthb.customplayer.player.MediaConstantsEnum
import kotlinx.android.synthetic.main.activity_capture_video.*

/**
 * 用opengles 渲染camera数据,用x264 编码,用rtmp 推送流数据
 */
class RtmpGLESActivity : AppCompatActivity(){
    private val mMuxer by lazy { object : CustomMediaRecorder(){
        override fun getMuxerType(): MediaConstantsEnum {
            return MediaConstantsEnum.MUXER_RTMP
        }
    } }
    private val cameraSurfaceHelper by lazy { Camera2SurfaceHelper(this,mMuxer) }
    private val mAudioRecorder by lazy { AudioRecorder() }


    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        setContentView(R.layout.activity_capture_video)
        keepScreenOn()
        mRecordButton.setStartListener {
            mMuxer.native_startEncode()
        }
        mRecordButton.setStopListener {
            mMuxer.native_stopEncode()
        }
        mAudioRecorder.setOnAudioFrameCapturedListener { audioData, ret ->
            mMuxer.native_audioData(audioData,ret)
        }
        mGLESMuxerSurface.init(mMuxer,true)
        cameraSurfaceHelper.init()

        val liveRtmpUrl = "rtmp://1.14.99.52:1935/live/windowsPush"
        mAudioRecorder.startCapture()
        mMuxer.native_configAudioParams(AudioConfiguration.DEFAULT_FREQUENCY,AudioConfiguration.DEFAULT_CHANNEL_COUNT)
        mMuxer.native_initEncode(liveRtmpUrl)
    }

    override fun onDestroy() {
        mAudioRecorder.stopCapture()
        cameraSurfaceHelper.onDestroy()
        mMuxer.destroy()
        cameraSurfaceHelper.release()
        super.onDestroy()
    }

    override fun onPause() {
        super.onPause()
    }

}
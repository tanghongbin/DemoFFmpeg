package com.testthb.customplayer.opus

import androidx.appcompat.app.AppCompatActivity
import android.os.Bundle
import com.ruiyi.mvvmlib.permission.EasyPermission
import com.testthb.avutils.audio.recoder.AudioRecorder
import com.testthb.customplayer.R
import kotlinx.android.synthetic.main.activity_opus_operation.*

/***
 * opus 的编解码，播放和录制一起
 */
class OpusOperationActivity : AppCompatActivity() {
    val opusHelper = OpusHelper()
    private val mAudioRecorder by lazy { AudioRecorder() }
    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        setContentView(R.layout.activity_opus_operation)
        EasyPermission.init(this)
            .permissions(android.Manifest.permission.WRITE_EXTERNAL_STORAGE,
            android.Manifest.permission.RECORD_AUDIO)
            .request { allGranted, grantedList, deniedList ->
                init()
            }
    }

    private fun init() {
        opusHelper.initOpus()
        startEncode.setOnClickListener {
            opusHelper.startEncode()
    //            mAudioRecorder.startCapture()
        }
        stopEncode.setOnClickListener {
            opusHelper.stopEncode()
        }
        startPlay.setOnClickListener {
            opusHelper.startPlayOpus()
        }
    }

    override fun onDestroy() {
        opusHelper.destroy();
        super.onDestroy()
    }
}
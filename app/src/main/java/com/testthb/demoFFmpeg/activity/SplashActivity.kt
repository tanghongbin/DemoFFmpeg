package com.testthb.demoFFmpeg.activity

import android.os.Bundle
import androidx.appcompat.app.AppCompatActivity
import com.ruiyi.mvvmlib.permission.EasyPermission
import com.testthb.demoFFmpeg.R
import kotlinx.android.synthetic.main.activity_splash.*
import top.oply.opuslib.OpusService
import top.oply.opuslib.Utils

class SplashActivity : AppCompatActivity() {
    val fileName = "/storage/emulated/0/ffmpegtest/opus/test_valid.ops"
    var isRecording = false
    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        setContentView(R.layout.activity_splash)
        clickDecode.setOnClickListener {
            EasyPermission.init(this)
                .permissions(android.Manifest.permission.READ_EXTERNAL_STORAGE,
                    android.Manifest.permission.RECORD_AUDIO)
                .request { allGranted, grantedList, deniedList ->
                    val result = Utils.isFileExist(fileName)
                    OpusService.play(this,fileName)
                }
        }
        clickRecord.setOnClickListener {
            isRecording = !isRecording
            clickRecord.text = if (isRecording) "停止录制" else "开始录制"
            if (isRecording) {
                OpusService.record(this,fileName)
            } else {
                OpusService.stopRecording(this)
            }
        }
        playPcm.setOnClickListener {

        }
    }
}
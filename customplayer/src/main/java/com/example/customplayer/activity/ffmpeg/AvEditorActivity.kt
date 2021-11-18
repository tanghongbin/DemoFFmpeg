package com.example.customplayer.activity.ffmpeg

import android.content.pm.ActivityInfo
import android.os.Build
import androidx.appcompat.app.AppCompatActivity
import android.os.Bundle
import android.view.View
import android.widget.SeekBar
import androidx.annotation.RequiresApi
import com.coder.ffmpeg.utils.FFmpegUtils
import com.example.common_base.utils.Constants.IMG_DIR
import com.example.common_base.utils.FileUtils
import com.example.common_base.utils.log
import com.example.common_base.utils.showViews
import com.example.common_base.utils.toastSafe
import com.example.customplayer.R
import com.example.customplayer.interfaces.*
import com.example.customplayer.player.CustomMediaController
import com.example.customplayer.util.CustomFFmpegCmdUtils
import com.example.customplayer.util.getRamdowVideoPath
import com.example.customplayer.util.runFFmpegCommand
import kotlinx.android.synthetic.main.activity_player_detail.*
import java.io.File
import kotlin.random.Random

/***
 * 直接用mediaplayer播放
 */
class AvEditorActivity : AppCompatActivity(){
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
        showViews(mCutVideo,mWaterMask)
        mWaterMask.setOnClickListener {
            val picPath = "${IMG_DIR}/container.jpg"
            val outputPath = getRamdowVideoPath("watermask")
            if (!FileUtils.isFileExist(outputPath)){
                File(outputPath).createNewFile()
            }
            val command = FFmpegUtils.addWaterMark(mUrl,picPath,outputPath)
//            val command = "ffmpeg -i $mUrl -i $picPath -filter_complex overlay=600:15 -acodec copy -b:v 2500k $outputPath"
            runFFmpegCommand(command){
                log("添加水印成功")
                convertSuccess(outputPath)
            }
        }
        mCutVideo.setOnClickListener {
            val outputPath = getRamdowVideoPath("cut")
            FileUtils.createFileIfNotExist(outputPath)
            val command = CustomFFmpegCmdUtils.cutVideo(mUrl,0,10,outputPath)
            runFFmpegCommand(command) {
                convertSuccess(outputPath)
            }
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
        log("打印当前时间java:${System.currentTimeMillis()}")
    }

    private fun convertSuccess(outputPath: String) {
        mPlayer.native_setDataUrl(outputPath)
        mPlayer.native_prepare()
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
                mPlayer.native_replay()
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
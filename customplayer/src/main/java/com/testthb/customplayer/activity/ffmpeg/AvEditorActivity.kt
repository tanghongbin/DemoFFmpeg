package com.testthb.customplayer.activity.ffmpeg

import android.content.pm.ActivityInfo
import android.os.Build
import android.os.Bundle
import android.os.Handler
import android.view.SurfaceHolder
import android.widget.SeekBar
import androidx.annotation.RequiresApi
import androidx.appcompat.app.AppCompatActivity
import com.coder.ffmpeg.utils.FFmpegUtils
import com.testthb.common_base.utils.Constants.IMG_DIR
import com.testthb.common_base.utils.FileUtils
import com.testthb.common_base.utils.log
import com.testthb.common_base.utils.showViews
import com.testthb.common_base.utils.toastSafe
import com.testthb.customplayer.R
import com.testthb.customplayer.interfaces.*
import com.testthb.customplayer.player.CustomMediaController
import com.testthb.customplayer.util.CustomFFmpegCmdUtils
import com.testthb.customplayer.util.getRamdowVideoPath
import com.testthb.customplayer.util.runFFmpegCommand
import kotlinx.android.synthetic.main.activity_player_detail.*
import java.io.File
import kotlin.random.Random

/***
 * 直接用mediaplayer播放
 */
class AvEditorActivity : AppCompatActivity(), SurfaceHolder.Callback {
    private val mUrl by lazy {
        intent.getStringExtra("url") ?: ""
//        Constants.MP4_PLAY_PATH
    }
    private var isPlaying = true
    private val mPlayer by lazy { CustomMediaController(playerType = 2) }
    @RequiresApi(Build.VERSION_CODES.O)
    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        setContentView(R.layout.activity_player_detail)
        mGLSurface.holder.addCallback(this)
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
                mPlayer.native_resetPlay()
                convertSuccess(outputPath)
            }
        }
        mCutVideo.setOnClickListener {
            val outputPath = getRamdowVideoPath("cut")
            FileUtils.createFileIfNotExist(outputPath)
            val command = CustomFFmpegCmdUtils.cutVideo(mUrl,0,10,outputPath)
            runFFmpegCommand(command) {
                mPlayer.native_resetPlay()
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

            }

            override fun onStartTrackingTouch(seekBar: SeekBar?) {
            }

            override fun onStopTrackingTouch(seekBar: SeekBar?) {
                mPlayer.native_seekTo(seekBar?.progress ?: 0)
            }

        })
        setupListener()
        convertSuccess(mUrl)
        log("打印当前时间java:${System.currentTimeMillis()}")
    }

    private fun convertSuccess(outputPath: String) {
        Handler().postDelayed({
            mPlayer.native_setDataUrl(outputPath)
            mPlayer.native_prepare()
        },3000)
        log("准备开始播放了")
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
            @RequiresApi(Build.VERSION_CODES.O)
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

    override fun surfaceCreated(holder: SurfaceHolder) {
        mPlayer.native_setNativeWindow(holder.surface)
    }

    override fun surfaceChanged(holder: SurfaceHolder, format: Int, width: Int, height: Int) {

    }

    override fun surfaceDestroyed(holder: SurfaceHolder) {
        mPlayer.native_deleteNativeWindow()
    }

}
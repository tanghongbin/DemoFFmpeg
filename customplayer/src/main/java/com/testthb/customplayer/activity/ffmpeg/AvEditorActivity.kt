package com.testthb.customplayer.activity.ffmpeg

import android.content.pm.ActivityInfo
import android.os.Build
import android.os.Bundle
import android.widget.SeekBar
import androidx.annotation.RequiresApi
import androidx.appcompat.app.AppCompatActivity
import com.coder.ffmpeg.utils.FFmpegUtils
import com.testthb.common_base.utils.Constants.FILD_DIR
import com.testthb.common_base.utils.Constants.IMG_DIR
import com.testthb.common_base.utils.FileUtils
import com.testthb.common_base.utils.log
import com.testthb.common_base.utils.showViews
import com.testthb.common_base.utils.toastSafe
import com.testthb.customplayer.R
import com.testthb.customplayer.interfaces.*
import com.testthb.customplayer.player.CustomMediaPlayer
import com.testthb.customplayer.util.CustomFFmpegCmdUtils
import com.testthb.customplayer.util.getRamdowVideoPath
import com.testthb.customplayer.util.runFFmpegCommand
import kotlinx.android.synthetic.main.activity_player_detail.*
import tutorial.AddressBookOuterClass
import java.io.File
import kotlin.random.Random

/***
 * 直接用mediaplayer播放
 */
class AvEditorActivity : AppCompatActivity(){
    private val mUrl by lazy {
        intent.getStringExtra("url") ?: ""
//        Constants.MP4_PLAY_PATH00
    }
    private var isPlaying = true
    private val mPlayer by lazy { CustomMediaPlayer() }
    @RequiresApi(Build.VERSION_CODES.O)
    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        setContentView(R.layout.activity_player_detail)
        mGLSurface.init(mPlayer)
        AddressBookOuterClass.AddressBook.newBuilder()
            .setPrice(1.0)
            .build()
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
        showViews(mCutVideo,mWaterMask,mAddEfforts)
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
        mAddEfforts.setOnClickListener {
            val url = "${FILD_DIR}/special_efforts/star1.mp4"
            mPlayer.native_applyEfforts(url)
        }
        mSeekbar.setOnSeekBarChangeListener(object : SeekBar.OnSeekBarChangeListener{
            override fun onProgressChanged(seekBar: SeekBar?, progress: Int, fromUser: Boolean) {

            }

            override fun onStartTrackingTouch(seekBar: SeekBar?) {
            }

            override fun onStopTrackingTouch(seekBar: SeekBar?) {
                log("打印progress:${seekBar?.progress}")
                mPlayer.native_seekTo(seekBar?.progress ?: 0)
            }

        })
        setupListener()
        convertSuccess(mUrl)
        log("打印当前时间java:${System.currentTimeMillis()}")
    }

    private fun convertSuccess(outputPath: String) {
        mPlayer.native_setDataUrl(outputPath)
        mPlayer.native_prepare()
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
//                mPlayer.native_replay()
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

}
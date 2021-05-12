package com.example.democ.ffmpeg

import android.app.Activity
import android.content.Intent
import android.net.Uri
import android.os.Bundle
import android.widget.Toast
import androidx.appcompat.app.AppCompatActivity
import com.example.democ.R
import com.example.democ.audio.log
import com.example.democ.render.FFmpegRender
import com.example.common_base.utils.Constants
import kotlinx.android.synthetic.main.activity_f_fmpeg_encode.*
import kotlinx.coroutines.*
import java.io.File
import java.util.*

/**
 *
 * author : tanghongbin
 *
 * date   : 2020/11/13 17:43
 *
 * desc   : 转换yuv到rgb，jpg，png
 *
 **/
class FFmpegConvertYuvActivity : AppCompatActivity() {

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        setContentView(R.layout.activity_f_fmpeg_encode)


        val render = FFmpegRender()

//        val MP4_PLAY_PATH = Environment.getExternalStorageDirectory().absolutePath + "/video.mp4"
//        log("打印播放地址:${MP4_PLAY_PATH}")

        button.setOnClickListener {
            mImage.setImageDrawable(null)
            val start = System.currentTimeMillis();
            val path = render.encodeYuvToImage(Constants.YUV_PATH)
            log("打印返回的地址:${path} 耗时:${System.currentTimeMillis() - start}")
            mImage.setImageURI(Uri.fromFile(File(path)))
        }

        button2.setOnClickListener {
            mImage.setImageDrawable(null)
            val start = System.currentTimeMillis();
//            render.swsPng()
            mImage.setImageURI(Uri.fromFile(File(Constants.SWS_CONVERT_PNG)))
        }

        button3.setOnClickListener {
            render.native_yuv2rgb();
        }

        button4.setOnClickListener {

            val job = Job()
            val couroutine = CoroutineScope(job)
            couroutine.launch(Dispatchers.IO) {
                log("转换开始")
                val start = System.currentTimeMillis()
                val timer = Timer()
                timer.schedule(object : TimerTask() {
                    override fun run() {
                        runOnUiThread {
                            mText.text =
                                String.format("${(System.currentTimeMillis() - start) / 1000.0f}s")
                        }
                    }
                }, 0, 1000)
//                val path = MuxerManager.MP4_PLAY_BIG_PATH
                val path = Constants.FLV_DOWNLOAD_MY_LOGO
//                val path = MuxerManager.MP4_DOWNLOAD_FUNNY_VIDEO
//                val path = MuxerManager.MP4_PLAY_PATH
                val resultPath = render.native_changeOutputFormat(path)
                log("打印返回的地址:${resultPath}")
                timer.cancel()
                log("总共转换耗时:${System.currentTimeMillis() - start}")
                withContext(Dispatchers.Main) {
                    if (resultPath.isNullOrEmpty()) {
                        toast("转换失败")
                    } else {
                        toast("编码陈宫")
                        delay(3000)
                        startActivity(
                            Intent(
                                this@FFmpegConvertYuvActivity,
                                FFmpegNativeWindowActivity::class.java
                            ).apply {
                                putExtra("path", resultPath)
                            })
                    }
                }
            }
        }

        button5.setOnClickListener {
            //            val path = MuxerManager.MP4_PLAY_BIG_PATH
//            val path = MuxerManager.FLV_DOWNLOAD_MY_LOGO
//                val path = MuxerManager.MP4_DOWNLOAD_FUNNY_VIDEO
            val path = Constants.MP4_PLAY_PATH
            render.splitAudioAndVideo(path)
        }

        button6.setOnClickListener {
            render.native_muxerAudioAndVideo()
        }

        button7.setOnClickListener {
            render.native_testReadFile()
        }
    }
}


fun Activity.toast(str: String) {
    Toast.makeText(this, str, Toast.LENGTH_SHORT).show()
}
package com.example.democ.ffmpeg

import android.content.Intent
import android.graphics.Bitmap
import android.graphics.BitmapFactory
import android.media.MediaMuxer
import android.net.Uri
import androidx.appcompat.app.AppCompatActivity
import android.os.Bundle
import android.os.Environment
import com.example.democ.R
import com.example.democ.audio.MuxerManager
import com.example.democ.audio.MuxerManager.Companion.SWS_CONVERT_PNG
import com.example.democ.audio.log
import com.example.democ.opengles.NativeRender
import com.example.democ.render.FFmpegRender
import kotlinx.android.synthetic.main.activity_f_fmpeg_encode.*
import java.io.File
import java.io.FileInputStream

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
            val path = render.encodeYuvToImage(MuxerManager.YUV_PATH)
            log("打印返回的地址:${path} 耗时:${System.currentTimeMillis() - start}")
            mImage.setImageURI(Uri.fromFile(File(path)))
        }

        button2.setOnClickListener {
            mImage.setImageDrawable(null)
            val start = System.currentTimeMillis();
//            render.swsPng()
            mImage.setImageURI(Uri.fromFile(File(SWS_CONVERT_PNG)))
        }

        button3.setOnClickListener {
            render.native_yuv2rgb();
        }

        button4.setOnClickListener {
            val path = MuxerManager.MP4_PATH
            var resultPath = render.native_changeOutputFormat(path)
            resultPath = ""
            log("打印返回的地址:${resultPath}")
            startActivity(Intent(this,FFmpegNativeWindowActivity::class.java).apply {
                putExtra("path",resultPath)
            })
        }
    }
}

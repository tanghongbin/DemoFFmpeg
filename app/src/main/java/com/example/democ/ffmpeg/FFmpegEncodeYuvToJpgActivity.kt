package com.example.democ.ffmpeg

import android.graphics.Bitmap
import android.graphics.BitmapFactory
import android.net.Uri
import androidx.appcompat.app.AppCompatActivity
import android.os.Bundle
import android.os.Environment
import com.example.democ.R
import com.example.democ.audio.MuxerManager.Companion.SWS_CONVERT_PNG
import com.example.democ.audio.log
import com.example.democ.opengles.NativeRender
import com.example.democ.render.FFmpegRender
import kotlinx.android.synthetic.main.activity_f_fmpeg_encode.*
import java.io.File
import java.io.FileInputStream

class FFmpegEncodeYuvToJpgActivity : AppCompatActivity() {

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        setContentView(R.layout.activity_f_fmpeg_encode)

        val testGoodPath = "/storage/emulated/0/test.jpg"


        val render = FFmpegRender()

//        val MP4_PLAY_PATH = Environment.getExternalStorageDirectory().absolutePath + "/video.mp4"
//        log("打印播放地址:${MP4_PLAY_PATH}")

        button.setOnClickListener {
            mImage.setImageDrawable(null)
            val start = System.currentTimeMillis();
            val path = render.encodeYuvToImage(1)
            log("打印返回的地址:${path} 耗时:${System.currentTimeMillis() - start}")
            mImage.setImageURI(Uri.fromFile(File(path)))
        }

        button2.setOnClickListener {
            mImage.setImageDrawable(null)
            val start = System.currentTimeMillis();
//            render.swsPng()
            mImage.setImageURI(Uri.fromFile(File(SWS_CONVERT_PNG)))
        }

        Thread{
            val startMillion = System.currentTimeMillis()

            val file = File("/storage/emulated/0/ffmpegtest/test_png_out_file.png")
            val input = FileInputStream(file)
            val bytearray = input.readBytes()
            log("最后计数:${bytearray.size} 总共耗时:${System.currentTimeMillis() - startMillion}")
        }.start()

        render.native_testReadFile()
    }
}

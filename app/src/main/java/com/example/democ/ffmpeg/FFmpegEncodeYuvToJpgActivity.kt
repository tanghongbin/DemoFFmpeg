package com.example.democ.ffmpeg

import android.graphics.Bitmap
import android.graphics.BitmapFactory
import android.net.Uri
import androidx.appcompat.app.AppCompatActivity
import android.os.Bundle
import android.os.Environment
import com.example.democ.R
import com.example.democ.audio.log
import com.example.democ.opengles.NativeRender
import com.example.democ.render.FFmpegRender
import kotlinx.android.synthetic.main.activity_f_fmpeg_encode.*
import java.io.File

class FFmpegEncodeYuvToJpgActivity : AppCompatActivity() {

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        setContentView(R.layout.activity_f_fmpeg_encode)

        val testGoodPath = "/storage/emulated/0/test.jpg"

        val start = System.currentTimeMillis();
        val path = FFmpegRender().encodeYuvToImage("")
        log("打印返回的地址:${path} 耗时:${System.currentTimeMillis() - start}")
        mImage.setImageURI(Uri.fromFile(File(path)))

//        val MP4_PLAY_PATH = Environment.getExternalStorageDirectory().absolutePath + "/video.mp4"
//        log("打印播放地址:${MP4_PLAY_PATH}")
    }
}

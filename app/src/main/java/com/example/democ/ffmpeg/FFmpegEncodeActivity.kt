package com.example.democ.ffmpeg

import android.net.Uri
import androidx.appcompat.app.AppCompatActivity
import android.os.Bundle
import android.os.Environment
import com.example.democ.R
import com.example.democ.audio.log
import com.example.democ.opengles.NativeRender
import kotlinx.android.synthetic.main.activity_f_fmpeg_encode.*
import java.io.File

class FFmpegEncodeActivity : AppCompatActivity() {

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        setContentView(R.layout.activity_f_fmpeg_encode)
        val start = System.currentTimeMillis();
        val path = NativeRender().encodeYuvToImage("file:///android_asset/image/YUV_Image_840x1074.NV21")
        log("打印返回的地址:${path} 耗时:${System.currentTimeMillis() - start}")
        mImage.setImageURI(Uri.fromFile(File(path)))

        val MP4_PLAY_PATH = Environment.getExternalStorageDirectory().absolutePath + "/video.mp4"
        log("打印播放地址:${MP4_PLAY_PATH}")
    }
}

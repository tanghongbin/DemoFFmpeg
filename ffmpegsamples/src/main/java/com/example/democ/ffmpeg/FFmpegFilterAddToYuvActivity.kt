package com.example.democ.ffmpeg

import android.net.Uri
import androidx.appcompat.app.AppCompatActivity
import android.os.Bundle
import com.example.democ.R
import com.example.democ.render.FFmpegRender
import com.example.common_base.utils.Constants
import kotlinx.android.synthetic.main.activity_f_fmpeg_filter_add_to_yuv.*
import java.io.File

class FFmpegFilterAddToYuvActivity : AppCompatActivity() {

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        setContentView(R.layout.activity_f_fmpeg_filter_add_to_yuv)
        val str = FFmpegRender().native_addFilterToYuv(Constants.YUV_PATH)
        mImage.setImageURI(Uri.fromFile(File(str)))
    }
}

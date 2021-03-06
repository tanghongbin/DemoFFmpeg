package com.example.democ.ffmpeg

import android.content.Intent
import android.os.Bundle
import android.widget.ArrayAdapter
import androidx.appcompat.app.AppCompatActivity
import com.example.democ.R
import com.example.democ.hwencoder.FFmpegHwEncodeMuxerActivity
import kotlinx.android.synthetic.main.dialog_siamples.*


class FFmpegNavigationActivity : AppCompatActivity() {

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        setContentView(R.layout.activity_list)
        val arrayStr = arrays.map {
            it.simpleName
        }
        val adapter = ArrayAdapter<String>(this, android.R.layout.simple_list_item_1, arrayStr)
        mListView.adapter = adapter
        mListView.setOnItemClickListener { parent, view, position, id ->

                val classes = arrays[position]
                startActivity(Intent(this@FFmpegNavigationActivity, classes))


        }
//        if (getLastIndex() != -1 && getLastIndex() != 7){
//            startActivity(Intent(this@SplashActivity,arrays[getLastIndex()]))
//        }
    }

    private val arrays = arrayOf(
        FFmpegAboutActivity::class.java,
        FFmpegConvertYuvActivity::class.java,
        FFmpegOpenGLWindowActivity::class.java,
        FFmpegVideoActivity::class.java,
        FFmpegTextureActivity::class.java,
        FFmpegNativeWindowActivity::class.java,
        FFmpegEncodeAudioActivity::class.java,
        FFmpegEncodeVideoActivity::class.java,
        FFmpegFilterAddToYuvActivity::class.java,
        FFmpegEncodeAVToMp4Activity::class.java,
        FFmpegHwEncodeMuxerActivity::class.java,
        FFmpegRtmpNativeActivity::class.java,
        FFmpegRtmpJavaHwActivity::class.java,
        FFmpegTestNewCameraHelperActivity::class.java
    )



}

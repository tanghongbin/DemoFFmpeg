package com.example.democ.ffmpeg

import android.content.Context
import android.content.Intent
import androidx.appcompat.app.AppCompatActivity
import android.os.Bundle
import android.widget.ArrayAdapter
import com.example.democ.R
import com.example.democ.activity.AudioTestActivity
import com.example.democ.activity.ThreadTestActivity
import com.example.democ.opengles.*
import com.example.democ.requestCustomPermissions
import kotlinx.android.synthetic.main.activity_splash.*

class FFmpegNavigationActivity : AppCompatActivity() {

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        setContentView(R.layout.activity_splash)
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
        FFmpegEncodeActivity::class.java,
        FFmpegOpenGLActivity::class.java,
        FFmpegVideoActivity::class.java,
        FFmpegTextureActivity::class.java,
        FFmpegNativeWindowActivity::class.java,
        FFmpegEncodeAudioActivity::class.java,
        FFmpegEncodeVideoActivity::class.java
    )


}

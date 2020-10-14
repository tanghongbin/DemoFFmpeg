package com.example.democ

import android.content.Context
import android.content.Intent
import androidx.appcompat.app.AppCompatActivity
import android.os.Bundle
import android.widget.ArrayAdapter
import com.example.democ.activity.AudioTestActivity
import com.example.democ.activity.ThreadTestActivity
import com.example.democ.ffmpeg.FFmpegEncodeActivity
import com.example.democ.ffmpeg.FFmpegOpenGLActivity
import com.example.democ.ffmpeg.FFmpegTextureActivity
import com.example.democ.ffmpeg.FFmpegVideoActivity
import com.example.democ.opengles.*
import kotlinx.android.synthetic.main.activity_splash.*

class SplashActivity : AppCompatActivity() {

    private val arrays = arrayOf(
        AudioTestActivity::class.java,
        ThreadTestActivity::class.java,
        FFmpegEncodeActivity::class.java,
        FFmpegOpenGLActivity::class.java,
        FFmpegVideoActivity::class.java,

        EmptyActivity::class.java,
        NativeEglTestActivity::class.java,
        NativeOpenGLESActiivty::class.java,
        OpenGLImageReaderActivity::class.java,
        TestThreadActivity::class.java,
        FFmpegTextureActivity::class.java

    )
    fun putIndex(position:Int){
        getSharedPreferences("demoC", Context.MODE_PRIVATE).edit()
            .putInt("position",position).apply()
    }

    fun getLastIndex():Int{
        return getSharedPreferences("demoC", Context.MODE_PRIVATE)
            .getInt("position",-1)
    }

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        setContentView(R.layout.activity_splash)
        val arrayStr = arrays.map {
            it.simpleName
        }
        val adapter = ArrayAdapter<String>(this,android.R.layout.simple_list_item_1,arrayStr)
        mListView.adapter = adapter
        mListView.setOnItemClickListener { parent, view, position, id ->
            val classes = arrays[position]
            startActivity(Intent(this@SplashActivity,classes))
            putIndex(position)
        }
//        if (getLastIndex() != -1 && getLastIndex() != 7){
//            startActivity(Intent(this@SplashActivity,arrays[getLastIndex()]))
//        }
    }
}

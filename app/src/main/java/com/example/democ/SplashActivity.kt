package com.example.democ

import android.content.Context
import android.content.Intent
import androidx.appcompat.app.AppCompatActivity
import android.os.Bundle
import android.widget.ArrayAdapter
import com.example.common_base.utils.requestCustomPermissions
import com.example.democ.activity.AudioTestActivity
import com.example.democ.activity.ThreadTestActivity
import com.example.democ.ffmpeg.*
import com.example.democ.opengles.*
import kotlinx.android.synthetic.main.activity_splash.*

class SplashActivity : AppCompatActivity() {

    private val arrays = arrayOf(
        MainActivity::class.java,
        AudioTestActivity::class.java,
        FFmpegNavigationActivity::class.java,
        OpenGLESNavigationActivity::class.java

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
            val name = if(it.javaClass == FFmpegNavigationActivity::class.java){
                "ffmpeg系列"
            } else {
                it.simpleName
            }
            name
        }
        val adapter = ArrayAdapter<String>(this,android.R.layout.simple_list_item_1,arrayStr)
        mListView.adapter = adapter
        mListView.setOnItemClickListener { parent, view, position, id ->
            requestCustomPermissions {
                val classes = arrays[position]
                startActivity(Intent(this@SplashActivity,classes))
                putIndex(position)
            }
        }
//        if (getLastIndex() != -1 && getLastIndex() != 7){
//            startActivity(Intent(this@SplashActivity,arrays[getLastIndex()]))
//        }
    }
}

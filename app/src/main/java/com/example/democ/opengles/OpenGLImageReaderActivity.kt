package com.example.democ.opengles

import androidx.appcompat.app.AppCompatActivity
import android.os.Bundle
import com.example.democ.R

class OpenGLImageReaderActivity : AppCompatActivity() {

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        setContentView(R.layout.activity_open_g_l_image_reader)
    }

    companion object{
        init {
//            System.loadLibrary("native-lib")
        }
    }
}

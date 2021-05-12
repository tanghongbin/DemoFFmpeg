package com.example.democ.activity

import androidx.appcompat.app.AppCompatActivity
import android.os.Bundle
import com.example.democ.R
import com.example.democ.audio.log
import com.example.democ.opengles.NativeRender
import java.util.concurrent.locks.ReentrantLock

class ThreadTestActivity : AppCompatActivity() {

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        setContentView(R.layout.activity_thread_test)


        NativeRender().testThread()


    }
}

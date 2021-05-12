package com.example.democ.opengles

import androidx.appcompat.app.AppCompatActivity
import android.os.Bundle
import com.example.democ.R

@Deprecated("")
class TestThreadActivity : AppCompatActivity() {

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        setContentView(R.layout.activity_test_thread)
        NativeRender().testThread()
    }
}

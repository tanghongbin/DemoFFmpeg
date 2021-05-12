package com.example.democ.opengles

import android.content.Intent
import android.os.Bundle
import android.widget.ArrayAdapter
import androidx.appcompat.app.AppCompatActivity
import com.example.democ.R
import com.example.democ.view.SampleDialog
import kotlinx.android.synthetic.main.activity_list.*


class OpenGLESNavigationActivity : AppCompatActivity() {

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        setContentView(R.layout.activity_list)
        val arrayStr = arrays.map {
            it.simpleName
        }
        val adapter = ArrayAdapter<String>(this, android.R.layout.simple_list_item_1, arrayStr)
        mListView.adapter = adapter
        mListView.setOnItemClickListener { parent, view, position, id ->
            if (arrays[position] == OpenGLESSampleActiivty::class.java){
                val dialog = SampleDialog(this,R.style.DialogStyle)
                dialog.mConfirm = {type ->
                    val classes = arrays[position]
                    startActivity(Intent(this@OpenGLESNavigationActivity, classes).apply {
                        putExtra("type",type)
                    })
                }
            dialog.show()
            } else {
                val classes = arrays[position]
                startActivity(Intent(this@OpenGLESNavigationActivity, classes))
            }
        }
//        if (getLastIndex() != -1 && getLastIndex() != 7){
//            startActivity(Intent(this@SplashActivity,arrays[getLastIndex()]))
//        }
    }

    private val arrays = arrayOf(
        OpenGLESSampleActiivty::class.java,
        OpenGLesEglCtxActiivty::class.java
    )



}

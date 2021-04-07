package com.example.democ.opengles

import androidx.appcompat.app.AppCompatActivity
import android.os.Bundle
import com.example.democ.R
import com.example.democ.audio.log
import com.example.democ.utils.Constants
import com.example.democ.utils.getFragmentStrByType
import com.example.democ.utils.getVertexStrByType
import com.example.democ.view.SampleDialog
import kotlinx.android.synthetic.main.activity_native_open_g_l_e_s_actiivty.*

class OpenGLESSampleActiivty : AppCompatActivity() {

    val render by lazy { NativeRender(intent.getIntExtra("type",1)) }

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        setContentView(R.layout.activity_native_open_g_l_e_s_actiivty)
        mSurface.init(this,render)
        log("打印测试地址:${Constants.TEST_JPG}")
//        mChangeSample.setOnClickListener {
//            val dialog = SampleDialog(this,R.style.DialogStyle)
//            dialog.mConfirm = {num ->
//                val vertexStr = getVertexStrByType(num)
//                val fragStr = getFragmentStrByType(num)
//                log("打印java层的字符串:\n\n${vertexStr} \n\n${fragStr}")
//                render.native_changeSamples(num,vertexStr,fragStr)
//            }
//            dialog.show()
//        }
    }

    override fun onDestroy() {
        render.native_OnUnInit()
        super.onDestroy()
    }

}

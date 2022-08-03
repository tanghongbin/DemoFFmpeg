package com.testthb.customplayer.activity

import android.graphics.Bitmap
import android.graphics.BitmapFactory
import android.os.Build
import android.os.Bundle
import android.view.View
import androidx.annotation.RequiresApi
import androidx.appcompat.app.AppCompatActivity
import androidx.lifecycle.ViewModelProvider
import com.testthb.common_base.utils.log
import com.testthb.customplayer.R
import java.util.*

class TestActivity : AppCompatActivity() {

    private val viewModel by lazy {
        ViewModelProvider(viewModelStore,
            ViewModelProvider.AndroidViewModelFactory.getInstance(application))
            .get(TestViewModel::class.java)
    }

    @RequiresApi(Build.VERSION_CODES.O)
    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        setContentView(R.layout.activity_test2)
//        val adapter = ListAdapter()
//        mRecyclePlayer.layoutManager = LinearLayoutManager(this)
//        mRecyclePlayer.adapter =
        val pngPath = "/storage/emulated/0/ffmpegtest/img/test1.png"
        val jpgPath = "/storage/emulated/0/ffmpegtest/img/test5.jpg"
        val options = BitmapFactory.Options()
        options.outConfig = Bitmap.Config.RGB_565
        val bitmapPng = BitmapFactory.decodeFile(pngPath,options)
        val bitmapJpg = BitmapFactory.decodeFile(jpgPath)
        val bitmapxhDpi = BitmapFactory.decodeResource(resources,R.mipmap.test1xh)
        val bitmapxxhDpi = BitmapFactory.decodeResource(resources,R.mipmap.test1xxh)
        val bitmapxxxhDpi = BitmapFactory.decodeResource(resources,R.mipmap.test1xxxh)
        log("打印png大小:${computeByte(bitmapPng.byteCount)}m   jpg:${computeByte(bitmapJpg.byteCount)}m")
        log("打印不同分辨率下的图片:xh-${bitmapxhDpi.byteCount.toMb()}   xxh-${bitmapxxhDpi.byteCount.toMb()}" +
                "    xxxh-${bitmapxxxhDpi.byteCount.toMb()}")

        val tree = TreeMap<String,String>()
        tree.put("3322","22")
        tree.put("333333","22")
        tree.put("33","22")

        tree.put("33333","22")

        log("打印tree:${tree}")
        viewModel.liveData.observe(this, androidx.lifecycle.Observer {
            log("打印tree data:${it}")
        })
    }



    fun computeByte(byteCount:Int):Double{
        return byteCount * 1.0 / 1024 /1024
    }

    override fun onResume() {
        super.onResume()
        log("打印 onResume")
    }

    fun test(){

    }

    companion object{
        fun test1(){

        }
    }

    fun testClick(view: View) {
        repeat(5) {
            viewModel.liveData.value = "value:${it}"
        }
    }
}

fun TestActivity.addNum(){
    test()
}

fun Int.toMb():Double{
    return this * 1.0 / 1024 /1024;
}
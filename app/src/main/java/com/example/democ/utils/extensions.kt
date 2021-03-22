package com.example.democ.utils

import android.content.Context
import com.example.democ.DemoApplication
import com.example.democ.audio.log
import java.io.File
import java.io.FileInputStream
import java.io.IOException
import java.nio.charset.Charset

fun getRandomStr(subdir:String,suffixName:String):String{
    val path = "/storage/emulated/0/ffmpegtest/${subdir}${System.currentTimeMillis()}${suffixName}"
    log("path:${path}")
    val file = File(path)
    if (!file.exists()){
        val result = file.createNewFile()
        log("path:${path}   ---文件是否创建成功:${result}")
    }
    return path
}

object TimeTracker{
    private var start = 0L
    private var threadId = 0L
    fun trackBegin(){
        if (threadId != 0L && threadId != Thread.currentThread().id){
            throw IllegalStateException("don't support in multiple thread")
        }
        threadId = Thread.currentThread().id
        start =  System.currentTimeMillis()
    }

    fun trackEnd(){
        log("TimeTracker total cost:${System.currentTimeMillis() - start}")
    }
}

fun getStrFromAssets(name:String):String{
    var str = ""
    try {
        val stream = DemoApplication.instance.assets.open(name)
        // size 为字串的长度 ，这里一次性读完
        val size: Int = stream.available()
        val buffer = ByteArray(size)
        stream.read(buffer)
        stream.close()
        str = String(buffer, Charset.defaultCharset())
    } catch (e: IOException) {
        // TODO Auto-generated catch block
        e.printStackTrace()
        return ""
    }
    return str
}
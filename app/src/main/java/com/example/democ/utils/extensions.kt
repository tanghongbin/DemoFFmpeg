package com.example.democ.utils

import com.example.democ.audio.log
import java.io.File

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
    fun trackBegin():Long{
        return System.currentTimeMillis()
    }

    fun trackEnd(start:Long){
        log("TimeTracker total cost:${System.currentTimeMillis() - start}")
    }
}
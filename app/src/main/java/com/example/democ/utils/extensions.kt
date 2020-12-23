package com.example.democ.utils

import com.example.democ.audio.log
import java.io.File
import java.lang.IllegalStateException

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
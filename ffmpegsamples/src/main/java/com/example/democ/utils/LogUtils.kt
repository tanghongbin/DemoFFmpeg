package com.example.democ.utils

import android.util.Log

object LogUtils {
    fun log(info:String?,tag:String = "DemoC"){
        Log.d(tag,info)
    }
    fun log(info:String?){
        log(info,"DemoC")
    }
}
package com.example.democ.utils

import android.content.Context.MODE_PRIVATE
import com.example.common_base.DemoApplication

object SpUtils {
    private val sharedPreferences = DemoApplication.instance.getSharedPreferences("demoCApp",MODE_PRIVATE)

    fun putString(name:String,value:String?){
        sharedPreferences.edit().putString(name,value).apply()
    }

    fun getString(name:String):String?{
        return sharedPreferences.getString(name,"")
    }
}
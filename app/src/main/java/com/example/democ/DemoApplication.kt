package com.example.democ

import android.app.Application
import com.libyuv.LibyuvUtil

class DemoApplication : Application() {
    companion object{
        lateinit var instance:Application
    }
    override fun onCreate() {
        super.onCreate()
        instance = this
    }
}
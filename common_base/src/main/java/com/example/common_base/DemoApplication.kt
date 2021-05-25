package com.example.common_base

import android.app.Application
import android.os.Environment
import com.example.common_base.utils.FileUtils
import com.squareup.leakcanary.LeakCanary

class DemoApplication : Application() {
    companion object{
        fun getInstance():Application{
            return instance_!!
        }
        private var instance_:Application? = null
    }
    override fun onCreate() {
        super.onCreate()
        instance_ = this
        val sdcardAssetsFolder = Environment.getExternalStorageDirectory().absolutePath + "/ffmpegtest/assets/glsl"
        FileUtils.deleteFile(sdcardAssetsFolder)
        FileUtils.makeFolders(sdcardAssetsFolder)
        FileUtils.copyFilesFromAssets(this,"glsl",sdcardAssetsFolder)
        LeakCanary.install(this)
    }
}
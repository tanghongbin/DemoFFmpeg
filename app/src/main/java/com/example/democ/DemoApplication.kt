package com.example.democ

import android.app.Application
import android.os.Environment
import com.example.democ.utils.FileUtils

class DemoApplication : Application() {
    companion object{
        lateinit var instance:Application
    }
    override fun onCreate() {
        super.onCreate()
        val sdcardAssetsFolder = Environment.getExternalStorageDirectory().absolutePath + "/ffmpegtest/assets/glsl"
        FileUtils.deleteFile(sdcardAssetsFolder)
        FileUtils.makeFolders(sdcardAssetsFolder)
        FileUtils.copyFilesFromAssets(this,"glsl",sdcardAssetsFolder)
        instance = this
    }
}
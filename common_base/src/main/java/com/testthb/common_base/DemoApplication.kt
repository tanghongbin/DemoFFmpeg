package com.testthb.common_base

import android.app.Application
import android.os.Environment
import android.os.Process
import com.tencent.bugly.crashreport.CrashReport
import com.tencent.bugly.crashreport.CrashReport.UserStrategy
import com.testthb.common_base.utils.Constants.bugly_id
import com.testthb.common_base.utils.FileUtils
import com.testthb.common_base.utils.SystemUtil
import com.testthb.common_base.utils.log

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
        FileUtils.copyFilesFromAssets(this, "glsl", sdcardAssetsFolder)
        initBugly()
    }

    private fun initBugly() {
        val context = applicationContext
        val packageName = context.packageName
        val processName: String = SystemUtil.getProcessName(Process.myPid())
        val strategy = UserStrategy(context)
        strategy.isUploadProcess = processName == null || processName == packageName
        log("打印上传线程${processName}")
        CrashReport.initCrashReport(context, bugly_id, BuildConfig.DEBUG, strategy)
//        CrashReport.testJavaCrash();
    }
}
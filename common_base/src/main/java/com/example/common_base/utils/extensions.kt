package com.example.common_base.utils

import android.Manifest
import android.app.Activity
import android.content.Context
import android.content.Intent
import android.graphics.Color
import android.opengl.GLSurfaceView
import android.os.Build
import android.util.Log
import android.view.SurfaceView
import android.view.View
import android.view.WindowManager
import android.widget.Toast
import androidx.appcompat.app.AppCompatActivity
import com.example.common_base.DemoApplication
import com.ruiyi.mvvmlib.permission.EasyPermission
import com.tbruyelle.rxpermissions2.RxPermissions
import kotlinx.coroutines.CoroutineScope
import kotlinx.coroutines.Dispatchers
import kotlinx.coroutines.Job
import kotlinx.coroutines.launch
import java.io.File
import java.io.IOException
import java.nio.charset.Charset

fun log(str:String,tag:String = "DemoC"){
    Log.d(tag,str)
}

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
        val stream = DemoApplication.getInstance().assets.open(name)
        // size 为字串的长度 ，这里一次性读完
        val size: Int = stream.available()
        val buffer = ByteArray(size)
        stream.read(buffer)
        stream.close()
        str = String(buffer, Charset.defaultCharset())
    } catch (e: IOException) {
        e.printStackTrace()
        return ""
    }
    return str
}



@Suppress("SameParameterValue")
fun getFragmentNameByType(sampleType: Int):String {
    return GlslDataCenter.getFragmentStrByType(sampleType)
}

/***
 * 片段字符串by type
 */
fun getFragmentStrByType(sampleType: Int):String {
    return getStrFromAssets(
        getFragmentNameByType(
            sampleType
        )
    )
}

fun getVertexNameByType(sampleType: Int):String {
    return GlslDataCenter.getVertexStrByType(sampleType)
}

fun getVertexStrByType(sampleType: Int):String {
    return getStrFromAssets(
        getVertexNameByType(
            sampleType
        )
    )
}

/**
@author 汤洪斌
@time 2019/3/19 0019 9:10
@version 1.0
@describe 保持屏幕敞亮
 */
fun AppCompatActivity.keepScreenOn() {
    window.addFlags(WindowManager.LayoutParams.FLAG_KEEP_SCREEN_ON)
}

/**
@author 汤洪斌
@time 2019/3/19 0019 9:10
@version 1.0
@describe 设置全屏
 */
fun AppCompatActivity.fullScreen() {
    if (Build.VERSION.SDK_INT >= 21) {
        val decorView = window.decorView
        val option = View.SYSTEM_UI_FLAG_LAYOUT_FULLSCREEN or View.SYSTEM_UI_FLAG_LAYOUT_STABLE
        decorView.systemUiVisibility = option
        window.statusBarColor = Color.TRANSPARENT
    }
}

fun AppCompatActivity.requestCustomPermissions(success: () -> Unit) {
    // 要检查授权
    EasyPermission.init(this)
        .permissions(
            Manifest.permission.CAMERA,
            Manifest.permission.WRITE_EXTERNAL_STORAGE,
            Manifest.permission.RECORD_AUDIO)
        .onForwardToSettings { deniedList ->
            showForwardToSettingsDialog(deniedList)
        }
        .request { allGranted, _, _ ->
            if (allGranted) {
                success()
            }
        }

}

fun SurfaceView.changeScreenSize(width:Int,height:Int){
    val params = layoutParams
    params.width = width
    params.height = height
    layoutParams = params
    requestLayout()
}

fun GLSurfaceView.changeScreenSize(width:Int,height:Int){
    val params = layoutParams
    params.width = width
    params.height = height
    layoutParams = params
    requestLayout()
}

fun Context.toastSafe(str:String?){
    val coroutineScope = CoroutineScope(Job())
    coroutineScope.launch(Dispatchers.Main) {
        Toast.makeText(DemoApplication.getInstance(),str,Toast.LENGTH_SHORT).show()
    }
}

fun Activity.createIntent(clazz: Class<*>):Intent{
    return Intent(this,clazz)
}
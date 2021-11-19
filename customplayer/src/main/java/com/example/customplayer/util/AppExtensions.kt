package com.example.customplayer.util

import android.app.Activity
import android.os.Handler
import android.widget.Toast
import com.coder.ffmpeg.call.IFFmpegCallBack
import com.coder.ffmpeg.jni.FFmpegCommand
import com.example.common_base.DemoApplication
import com.example.common_base.utils.log
import com.example.common_base.utils.runAsyncTask


fun runFFmpegCommand(command:String,success:() -> Unit){
    runFFmpegCommand(command.split(" ").toTypedArray(),success)
}

fun runFFmpegCommand(command:Array<String?>,success:() -> Unit){
    FFmpegCommand.setDebug(true)
    DemoApplication.getInstance().runAsyncTask({
        FFmpegCommand.runCmd(command,object : SimpleFFmpegCallback() {
            override fun onComplete() {
                Handler(DemoApplication.getInstance().mainLooper).post { success() }
                log("打印命令完成线程:${Thread.currentThread().name}")
            }

            override fun onProgress(progress: Int, pts: Long) {
                log("打印总进度:  ${progress}%")
            }
        })
    })
}


abstract class SimpleFFmpegCallback : IFFmpegCallBack {
      override fun onCancel() {}
      override fun onError(errorCode: Int, errorMsg: String?) {
          log("打印命令失败:${errorMsg}")
      }
      override fun onProgress(progress: Int, pts: Long) {
      }
      override fun onStart() {}
}

fun Activity.toast(str:String){
    Toast.makeText(this,str,Toast.LENGTH_SHORT).show()
}
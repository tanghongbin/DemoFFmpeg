package com.example.customplayer.interfaces

import android.opengl.GLSurfaceView
import android.view.SurfaceHolder
import android.view.SurfaceView
import javax.microedition.khronos.opengles.GL10

interface OnPreparedListener{
    fun onPrepared()
}
interface OnVideoSizeChangeListener{
    fun onVideoSize(width:Int,height:Int)
}
interface OnSeekProgressChangeListener{
    fun onProgress(progress:Int)
}
interface OnDurationListener{
    fun onDuration(min:Int,max:Int)
}
interface OnCompleteListener{
    fun onComplete()
}
interface OnErrorListener{
    fun onError(code:Int,str:String)
}
interface OnSurfaceCallListener : GLSurfaceView.Renderer {
    override fun onDrawFrame(gl: GL10?) {

    }
}


package com.example.democ.render

import android.graphics.BitmapFactory
import android.opengl.GLSurfaceView
import android.view.Surface
import com.example.democ.R
import com.example.democ.audio.MuxerManager
import com.example.democ.getAppContext
import com.example.democ.opengles.NativeRender
import java.nio.ByteBuffer
import javax.microedition.khronos.egl.EGLConfig
import javax.microedition.khronos.opengles.GL10

class FFmpegOpenGLRender : GLSurfaceView.Renderer{

    companion object{
        init {
            System.loadLibrary("native-player")
        }

        val IMAGE_FORMAT_RGBA = 0x01
        val IMAGE_FORMAT_NV21 = 0x02
        val IMAGE_FORMAT_NV12 = 0x03
        val IMAGE_FORMAT_I420 = 0x04
    }
    override fun onDrawFrame(gl: GL10?) {
        native_OnDrawFrame()
    }

    override fun onSurfaceChanged(gl: GL10?, width: Int, height: Int) {
        native_OnSurfaceChanged(width,height)
    }

    override fun onSurfaceCreated(gl: GL10?, config: EGLConfig?) {
        native_OnSurfaceCreated()
//        setImageByType(1)
        val url = MuxerManager.MP4_PLAY_PATH
        Thread{
            playMP4(url,null,3)
        }.start()

    }

    private fun setImageByType(type:Int) {
        when(type){
            1 -> {
                val bitmap = BitmapFactory.decodeResource(getAppContext().resources, R.mipmap.beautiful)
                val byteBuffer = ByteBuffer.allocate(bitmap.byteCount)
                bitmap.copyPixelsToBuffer(byteBuffer)
                native_SetImageData(IMAGE_FORMAT_RGBA, bitmap.width, bitmap.height, byteBuffer.array())
            }
            2 -> {
//                LoadNV21Image()
            }
        }
    }

    external fun native_OnSurfaceCreated()

    external fun native_OnSurfaceChanged(width:Int,height:Int)

    external fun native_OnDrawFrame()

    external fun playMP4(url:String, surface: Surface?, type: Int)

    external fun native_SetImageData(format:Int,width:Int,height:Int,byteArray: ByteArray)

}
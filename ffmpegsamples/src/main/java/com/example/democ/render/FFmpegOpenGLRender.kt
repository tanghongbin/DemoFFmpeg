package com.example.democ.render

import android.graphics.BitmapFactory
import android.opengl.GLSurfaceView
import android.view.Surface
import com.example.democ.R
import com.example.democ.getAppContext
import com.example.common_base.utils.Constants
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
//        log("java layer draw frame")
    }

    override fun onSurfaceChanged(gl: GL10?, width: Int, height: Int) {
        native_OnSurfaceChanged(width,height)
    }

    override fun onSurfaceCreated(gl: GL10?, config: EGLConfig?) {
        native_OnSurfaceCreated()
//        setImageByType(1)
        val url = Constants.MP4_PATH
//        val url = MuxerManager.MP4_PLAY_PATH
        // 1-音屏（不用指定），2-视频，3-yuv视频-不设置，4-opengl渲染
        playMP4(url,null,4)

    }

    private fun setImageByType(type:Int) {
        when(type){
            1 -> {
                val bitmap = BitmapFactory.decodeResource(getAppContext().resources, R.drawable.yifei)
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
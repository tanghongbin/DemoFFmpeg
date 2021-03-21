package com.example.democ.opengles

import android.graphics.BitmapFactory
import android.opengl.GLSurfaceView
import android.os.Handler
import android.os.Looper
import android.util.Log
import android.view.Surface
import com.example.democ.R
import com.example.democ.getAppContext
import com.example.democ.interfaces.MsgCallback
import java.io.IOException
import java.io.InputStream
import java.nio.ByteBuffer
import javax.microedition.khronos.egl.EGLConfig
import javax.microedition.khronos.opengles.GL10


class NativeRender
    : GLSurfaceView.Renderer
{

    private var mCall: MsgCallback? = null

    val IMAGE_FORMAT_RGBA = 0x01
    val IMAGE_FORMAT_NV21 = 0x02
    val IMAGE_FORMAT_NV12 = 0x03
    val IMAGE_FORMAT_I420 = 0x04

    private val mHandler = Handler(Looper.getMainLooper())

    companion object {
        // Used to load the 'native-lib' library on application startup.

        val TAG = "TAG"

        init {
            System.loadLibrary("native-render")
        }
        fun log(str: String) {
            Log.d(TAG, str)
        }
    }

    fun setMsgCall(block:MsgCallback){
        mCall = block
    }


    override fun onDrawFrame(gl: GL10?) {
        native_OnDrawFrame()
    }

    override fun onSurfaceChanged(gl: GL10?, width: Int, height: Int) {

        native_OnSurfaceChanged(width,height)
    }

    override fun onSurfaceCreated(gl: GL10?, config: EGLConfig?) {
        native_OnSurfaceCreated()
        setImageByType(1)
    }

    private fun setImageByType(type:Int) {
        when(type){
            1 -> {
                val bitmap = BitmapFactory.decodeResource(getAppContext().resources, R.drawable.lye)
                val byteBuffer = ByteBuffer.allocate(bitmap.byteCount)
                bitmap.copyPixelsToBuffer(byteBuffer)
                native_SetImageData(IMAGE_FORMAT_RGBA, bitmap.width, bitmap.height, byteBuffer.array())
            }
            2 -> {
                LoadNV21Image()
            }
        }
    }

    private fun LoadNV21Image() {
        var stream: InputStream? = null
        try {
            stream = getAppContext().assets.open("")
        } catch (e: IOException) {
            e.printStackTrace()
        }
        var lenght = 0
        try {
            lenght = stream?.available() ?: 0
            val buffer = ByteArray(lenght)
            stream?.read(buffer)
            native_SetImageData(IMAGE_FORMAT_NV21, 840, 1074, buffer)
        } catch (e: IOException) {
            e.printStackTrace()
        } finally {
            try {
                stream?.close()
            } catch (e: IOException) {
                e.printStackTrace()
            }
        }
    }

    private fun nativeMsgCallback(type: Int){
        log("java layer has received msg:${type} call:${mCall}")

        mHandler.post {
            mCall?.callback(type)
        }
    }

    /**
     * {"native_OnInit",           "()V",      (void *) (native_OnInit)},
    {"native_OnUnInit",         "()V",      (void *) (native_OnUnInit)},
    {"native_SetImageData",     "(III[B)V", (void *) (native_SetImageData)},
    {"native_OnSurfaceCreated", "()V",      (void *) (native_OnSurfaceCreated)},
    {"native_OnSurfaceChanged", "(II)V",    (void *) (native_OnSurfaceChanged)},
    {"native_OnDrawFrame",      "()V",      (void *) (native_OnDrawFrame)},
     */


    external fun native_OnInit()

    external fun native_OnUnInit()

    external fun native_SetImageData(format:Int,width:Int,height:Int,byteArray: ByteArray)

    external fun native_OnSurfaceCreated()

    external fun native_OnSurfaceChanged(width:Int,height:Int)

    external fun native_OnDrawFrame()

    external fun native_eglInit()

    external fun native_eglDraw()

    external fun native_eglUnInit()

    external fun native_eglSetImageData(width:Int,height:Int,byteArray: ByteArray)

    external fun getAvCodeInfo():String

    external fun encodeYuvToImage(url:String):String

    external fun testThread()

    external fun playMP4(url:String,surface: Surface?)

    external fun native_getVideoWidth():Int

    external fun native_getVideoHeight():Int

    external fun native_getTotalDuration():Long

    external fun native_seekPosition(position:Int)

    external fun native_pause()

    external fun native_resume()

    // ========  egl ctx =============

    external fun egl_init_2()

}
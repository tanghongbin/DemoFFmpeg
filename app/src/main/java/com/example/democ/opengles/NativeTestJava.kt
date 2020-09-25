package com.example.democ.opengles

import android.opengl.GLSurfaceView
import android.util.Log
import javax.microedition.khronos.egl.EGLConfig
import javax.microedition.khronos.opengles.GL10

class NativeTestJava : GLSurfaceView.Renderer{



    companion object {
        // Used to load the 'native-lib' library on application startup.

        val TAG = "TAG"

        init {
//            System.loadLibrary("native-test")
        }
        fun log(str: String) {
            Log.d(TAG, str)
        }
    }


    external fun native_OnInit()

    external fun native_OnUnInit()

    external fun native_setImageData(format:Int,width:Int,height:Int,byteArray: ByteArray)

    external fun native_onSurfaceCreated()

    external fun native_onSurfaceChanged(width:Int,height:Int)

    external fun native_onDrawFramed()




    override fun onDrawFrame(gl: GL10?) {
        native_onDrawFramed()
    }

    override fun onSurfaceChanged(gl: GL10?, width: Int, height: Int) {
        native_onSurfaceChanged(width,height)
    }

    override fun onSurfaceCreated(gl: GL10?, config: EGLConfig?) {
        native_onSurfaceCreated()
    }
}
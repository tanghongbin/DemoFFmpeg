package com.example.customplayer.player

import android.opengl.GLSurfaceView
import com.example.common_base.utils.log
import javax.microedition.khronos.egl.EGLConfig
import javax.microedition.khronos.opengles.GL10

@Suppress("KotlinJniMissingFunction")
class CustomPlayer : GLSurfaceView.Renderer {
    companion object{
        init {
            System.loadLibrary("native-customplayer")
        }
    }


    private fun receiveMsgFromJni(type:Int,arg1:Int,arg2:Int){
        log("打印参数:${type} $arg1 $arg2")
    }






    /***===================  native function **************************/

    external fun nativeGetInfo():String

    private external fun native_OnSurfaceCreated()

    private external fun native_OnSurfaceChanged(width:Int,height:Int)

    private external fun native_OnDrawFrame()

    external fun native_OnDestroy()

    /**====================  native function end **********************/


    /*******
     * ====================   callback ================================
     */
    override fun onDrawFrame(gl: GL10?) {
        native_OnDrawFrame()
    }

    override fun onSurfaceChanged(gl: GL10?, width: Int, height: Int) {
        native_OnSurfaceChanged(width,height)
    }

    override fun onSurfaceCreated(gl: GL10?, config: EGLConfig?) {
        native_OnSurfaceCreated()
    }
}
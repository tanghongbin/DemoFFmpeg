package com.example.customplayer.player

import android.opengl.GLSurfaceView
import android.os.Handler
import android.os.Looper
import android.os.Message
import com.example.common_base.utils.log
import com.example.customplayer.interfaces.*
import javax.microedition.khronos.egl.EGLConfig
import javax.microedition.khronos.opengles.GL10

@Suppress("KotlinJniMissingFunction", "FunctionName")
class CustomPlayer() : GLSurfaceView.Renderer {
    companion object{
        // communicate code
        private const val MSG_PREPARED = 1
        private const val MSG_VIDEO_SIZE_CHANGED = 2
        private const val MSG_SEEK_PROGRESS_CHANGED = 3
        private const val MSG_COMPLETE = 4
        private const val MSG_ERROR = 5
        private const val MSG_DURATION = 6

        init {
            System.loadLibrary("native-customplayer")
        }
    }
    init {
        native_init()
    }

    private var mNativePlayer:Long = 0L

    private var mOnPreparedListener: OnPreparedListener? = null
    private var mOnVideoSizeChangeListener: OnVideoSizeChangeListener? = null
    private var mOnSeekProgressChangeListener: OnSeekProgressChangeListener? = null
    private var mOnCompleteListener: OnCompleteListener? = null
    private var mOnErrorListener: OnErrorListener? = null
    private var mOnDurationListener: OnDurationListener? = null
    fun setOnDurationListener(listener:OnDurationListener){
        mOnDurationListener = listener
    }
    fun setPrepareListener(listener:OnPreparedListener){
        mOnPreparedListener = listener
    }
    fun setOnVideoSizeChangeListener(listener:OnVideoSizeChangeListener){
        mOnVideoSizeChangeListener = listener
    }
    fun setOnSeekProgressChangeListener(listener:OnSeekProgressChangeListener){
        mOnSeekProgressChangeListener = listener
    }
    fun setOnCompleteListener(listener:OnCompleteListener){
        mOnCompleteListener = listener
    }
    fun setOnErrorListener(listener:OnErrorListener){
        mOnErrorListener = listener
    }


    private val mHandler = object : Handler(Looper.getMainLooper()){
        override fun handleMessage(msg: Message?) {
            when(msg?.what){
                MSG_PREPARED -> mOnPreparedListener?.onPrepared()
                MSG_VIDEO_SIZE_CHANGED -> mOnVideoSizeChangeListener?.onVideoSize(msg.arg1,msg.arg2)
                MSG_SEEK_PROGRESS_CHANGED -> mOnSeekProgressChangeListener?.onProgress(msg.arg1)
                MSG_COMPLETE -> mOnCompleteListener?.onComplete()
                MSG_ERROR -> mOnErrorListener?.onError(msg.arg1,msg.obj.toString())
                MSG_DURATION -> mOnDurationListener?.onDuration(msg.arg1,msg.arg2)
            }
        }
    }


    private fun receivePlayerMsgFromJni(type:Int,arg1:Int,arg2:Int,msg:String){
        mHandler.obtainMessage(type,arg1,arg2,msg).sendToTarget()
    }

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

    /***===================  native function **************************/

    external fun nativeGetInfo():String

    private external fun native_OnSurfaceCreated()

    private external fun native_OnSurfaceChanged(width:Int,height:Int)

    private external fun native_OnDrawFrame()

    external fun native_OnDestroy()

    //================  播放 方法 ===========================
    external fun native_setDataUrl(url:String)

    external fun native_prepare()

    external fun native_start()

    external fun native_stop()

    external fun native_seekTo(seconds:Int)

    external fun native_init()


    /**====================  native function end **********************/
}
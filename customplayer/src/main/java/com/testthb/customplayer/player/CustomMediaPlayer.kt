package com.testthb.customplayer.player

import android.opengl.GLSurfaceView
import android.os.Bundle
import android.os.Handler
import android.os.Looper
import android.os.Message
import com.testthb.common_base.DemoApplication
import com.testthb.common_base.utils.log
import com.testthb.customplayer.interfaces.*
import javax.microedition.khronos.egl.EGLConfig
import javax.microedition.khronos.opengles.GL10

@Suppress("KotlinJniMissingFunction", "FunctionName")
// 1-播放器
class CustomMediaPlayer : GLSurfaceView.Renderer {

    private var isInit = false
    init {
        initByType()
    }

    // ffmpeg 播放，硬解码
    fun getPlayerType():MediaConstantsEnum = MediaConstantsEnum.PLAYER_HW


    private fun initByType() {
        if (isInit) return
        // ffmpeg 编码,硬编码,直播推流,短视频录制
        native_init_player(getPlayerType().value)
        isInit = true
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
        override fun handleMessage(msg: Message) {
            when(msg.what){
                MSG_PREPARED -> mOnPreparedListener?.onPrepared()
                MSG_VIDEO_SIZE_CHANGED -> mOnVideoSizeChangeListener?.onVideoSize(msg.arg1,msg.arg2)
                MSG_SEEK_PROGRESS_CHANGED -> mOnSeekProgressChangeListener?.onProgress(msg.arg1)
                MSG_COMPLETE -> mOnCompleteListener?.onComplete()
                MSG_ERROR -> mOnErrorListener?.onError(msg.arg1,msg.obj?.toString() ?: "")
                MSG_DURATION -> mOnDurationListener?.onDuration(msg.arg1,msg.arg2)
            }
        }
    }

    private fun receivePlayerMsgFromJni(type:Int,arg1:Int,arg2:Int,arg3:Long,msg:String){
        val sendMsg = mHandler.obtainMessage(type,arg1,arg2,msg)
        sendMsg.data = Bundle().apply {
            putLong("arg3",arg3)
        }
        sendMsg.sendToTarget()
    }

    /*******
     * --------------------- GLSurfaceView  callback START------------------------------
     */
    override fun onDrawFrame(gl: GL10?) {
        native_OnDrawFrame()
    }

    override fun onSurfaceChanged(gl: GL10?, width: Int, height: Int) {
        val orientation = DemoApplication.getInstance().resources.configuration.orientation
        native_OnSurfaceChanged(orientation,width,height)
    }

    override fun onSurfaceCreated(gl: GL10?, config: EGLConfig?) {
        native_OnSurfaceCreated()
    }

    /*******
     * ==================== GLSurfaceView  callback END ================================
     */

    /*******
     * ==================== SurfaceView  callback end ================================
     */

    fun destroy(){
        if (!isInit) return
        log("开始销毁")
        native_OnDestroy()
        isInit = false
    }

    /***=================== Surface native function **************************/

    private external fun native_OnSurfaceCreated()

    private external fun native_OnSurfaceChanged (oreration:Int,width:Int,height:Int)

    private external fun native_OnDrawFrame()

    private external fun native_OnDestroy()

    /****================ 播放器 player 播放 方法 ===========================**/

    external fun native_init_player(playerType:Int)

    external fun native_setDataUrl(url:String)

    external fun native_prepare()

    external fun native_start()

    external fun native_stop()

    external fun native_seekTo(seconds:Int)

    external fun native_replay()

    /***
     * 添加特效
     */
    external fun native_applyEfforts(url: String)

    /***
     * 重置播放器
     */
    external fun native_resetPlay()

    companion object{
        // communicate code
        private const val MSG_PREPARED = 1  // 准备完毕
        private const val MSG_VIDEO_SIZE_CHANGED = 2 // 播放视频尺寸改变
        private const val MSG_SEEK_PROGRESS_CHANGED = 3 // seek进度条改变
        private const val MSG_COMPLETE = 4 // 播放完成
        private const val MSG_ERROR = 5 // 播放错误
        private const val MSG_DURATION = 6 // 时长回调通知
        private const val MSG_MERGE_AV = 7 // 合并视频
        private const val MSG_CREATE_OES_TEXTURE_SUCCESS = 8 // 创建扩展纹理成功

        /**
         * 是播放器还是视频合成
         */
        private const val ROOT_TYPE_PLAY = 1
        private const val ROOT_TYPE_MUXER = 2

        init {
            System.loadLibrary("native-player")
        }
    }
}
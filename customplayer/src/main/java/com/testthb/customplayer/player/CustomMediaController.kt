package com.testthb.customplayer.player

import android.media.MediaSync
import android.opengl.GLSurfaceView
import android.os.Handler
import android.os.Looper
import android.os.Message
import android.view.SurfaceHolder
import com.testthb.common_base.DemoApplication
import com.testthb.common_base.utils.log
import com.testthb.customplayer.interfaces.*
import com.testthb.customplayer.util.getRamdowVideoPath
import javax.microedition.khronos.egl.EGLConfig
import javax.microedition.khronos.opengles.GL10

@Suppress("KotlinJniMissingFunction", "FunctionName")
// 1-播放器，2-muxer合成器
class CustomMediaController(rootType: Int = 1,muxerType:Int = 1,private val playerType: Int = 1) :
        GLSurfaceView.Renderer,SurfaceHolder.Callback {
    companion object{
        // communicate code
        private const val MSG_PREPARED = 1
        private const val MSG_VIDEO_SIZE_CHANGED = 2
        private const val MSG_SEEK_PROGRESS_CHANGED = 3
        private const val MSG_COMPLETE = 4
        private const val MSG_ERROR = 5
        private const val MSG_DURATION = 6
        private const val MSG_MERGE_AV = 7

        init {
            System.loadLibrary("native-mediacontroller")
        }
    }
    private var isInit = false
    init {
        initByType(rootType, muxerType)
    }

    private fun initByType(rootType: Int, muxerType: Int) {
        if (isInit) return
        when (rootType) {
            1 -> native_init_player(playerType) // 1- ffmpeg 播放，2 - 硬解码
            // 1-ffmpeg 编码,2-硬编码,3-直播推流,4-短视频录制
            2 -> native_init_muxer(muxerType)
        }
        isInit = true
    }

    private var mNativePlayer:Long = 0L
    private var mNativeMuxer:Long = 0L
    private var mNativeOutputHelper:Long = 0L
    private var mOnPreparedListener: OnPreparedListener? = null
    private var mOnVideoSizeChangeListener: OnVideoSizeChangeListener? = null
    private var mOnSeekProgressChangeListener: OnSeekProgressChangeListener? = null
    private var mOnCompleteListener: OnCompleteListener? = null
    private var mOnErrorListener: OnErrorListener? = null
    private var mOnDurationListener: OnDurationListener? = null
    private var mOnSurfaceCallListener: OnSurfaceCallListener? = null
    private var mOnAvMergeListener: OnAvMergeListener? = null
    fun setOnAvMergeListener(listener:OnAvMergeListener){
        mOnAvMergeListener = listener
    }
    fun setOnSurfaceCallListener(listener:OnSurfaceCallListener){
        mOnSurfaceCallListener = listener
    }
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
                MSG_ERROR -> mOnErrorListener?.onError(msg.arg1,msg.obj.toString())
                MSG_DURATION -> mOnDurationListener?.onDuration(msg.arg1,msg.arg2)
                MSG_MERGE_AV -> {
                    val str = msg.obj as String
                    val items = str.split(" ")
                    mOnAvMergeListener?.onMerge(items[0].toLong(),items[1],items[2])
                }
            }
        }
    }

    private fun receivePlayerMsgFromJni(type:Int,arg1:Int,arg2:Int,msg:String){
        mHandler.obtainMessage(type,arg1,arg2,msg).sendToTarget()
    }

    /*******
     * --------------------- GLSurfaceView  callback START------------------------------
     */
    override fun onDrawFrame(gl: GL10?) {
        native_OnDrawFrame()
    }

    override fun onSurfaceChanged(gl: GL10?, width: Int, height: Int) {
        val orientation = DemoApplication.getInstance().resources.configuration.orientation
        log("查看屏幕方向:${orientation}")
        mOnSurfaceCallListener?.onSurfaceChanged(gl,width,height)
        native_OnSurfaceChanged(orientation,width,height)
    }

    override fun onSurfaceCreated(gl: GL10?, config: EGLConfig?) {
        log("onSurfaceCreated success :${Thread.currentThread().name}")
        mOnSurfaceCallListener?.onSurfaceCreated(gl,config)
        native_OnSurfaceCreated()
    }

    /*******
     * ==================== GLSurfaceView  callback END ================================
     */


    /*******
     * --------------------- SurfaceView  callback start ------------------------------
     */

    override fun surfaceCreated(holder: SurfaceHolder) {

    }

    override fun surfaceChanged(holder: SurfaceHolder, format: Int, width: Int, height: Int) {

    }

    override fun surfaceDestroyed(holder: SurfaceHolder) {

    }

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

    external fun nativeGetInfo():String

    private external fun native_OnSurfaceCreated()

    private external fun native_OnSurfaceChanged (oreration:Int,width:Int,height:Int)

    private external fun native_OnDrawFrame()

    private external fun native_OnDestroy()

    /****================ 播放器 player 播放 方法 ===========================**/
    external fun native_setDataUrl(url:String)

    external fun native_prepare()

    external fun native_start()

    external fun native_stop()

    external fun native_seekTo(seconds:Int)

    external fun native_init_player(playerType:Int)

    external fun native_replay()

    /***
     * 添加特效
     */
    external fun native_applyEfforts(url: String)

    /***
     * 重置播放器
     */
    external fun native_resetPlay()

    /**====================  编码器  encoder 方法 **********************/

    /**
     * 1-ffmpeg 编码，2-硬编码
     */
    external fun native_init_muxer(type:Int)

    external fun native_onCameraFrameDataValible(type:Int,byteArray: ByteArray)

    // 1-开始录音
    external fun native_startEncode(path:String = getRamdowVideoPath())

    external fun native_stopEncode()

    external fun native_audioData(byteArray: ByteArray,length:Int)

    /***
     * 配置录音参数
     */
    external fun native_configAudioParams(sampleHz:Int,channels:Int)

    /***
     * 录制速率
     */
    external fun native_setSpeed(speed:Double)



}
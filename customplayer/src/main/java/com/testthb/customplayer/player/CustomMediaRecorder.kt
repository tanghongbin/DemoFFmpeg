package com.testthb.customplayer.player

import android.opengl.GLSurfaceView
import android.os.Bundle
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
// 视频合成
open class CustomMediaRecorder : GLSurfaceView.Renderer {

    private var isInit = false
    init {
        initByType()
    }

    open fun getMuxerType():MediaConstantsEnum = MediaConstantsEnum.MUXER_RECORD_HW //  // 1-ffmpeg 编码,2-硬编码,3-直播推流,4-短视频录制

    private fun initByType() {
        if (isInit) return
        native_init_muxer(getMuxerType().value)
        isInit = true
    }

    private var mNativeMuxer:Long = 0L
    private var mNativeOutputHelper:Long = 0L
    private var mOnErrorListener: OnErrorListener? = null
    private var mOnSurfaceCallListener: OnSurfaceCallListener? = null
    private var mOnAvMergeListener: OnAvMergeListener? = null
    private var mOnOESTextureListener: OnOESTextureListener? = null
    private var mOnDrawListener: OnDrawListener? = null
    fun setOnDrawListener(listener:OnDrawListener){
        mOnDrawListener = listener
    }
    fun setOnOESTextureListener(listener:OnOESTextureListener){
        mOnOESTextureListener = listener
    }
    fun setOnAvMergeListener(listener:OnAvMergeListener){
        mOnAvMergeListener = listener
    }
    fun setOnSurfaceCallListener(listener:OnSurfaceCallListener){
        mOnSurfaceCallListener = listener
    }
    fun setOnErrorListener(listener:OnErrorListener){
        mOnErrorListener = listener
    }


    private val mHandler = object : Handler(Looper.getMainLooper()){
        override fun handleMessage(msg: Message) {
            when(msg.what){
                MSG_ERROR -> mOnErrorListener?.onError(msg.arg1,msg.obj?.toString() ?: "")
                MSG_MERGE_AV -> {
                    val str = msg.obj as String
                    val items = str.split(" ")
                    mOnAvMergeListener?.onMerge(items[0].toLong(),items[1],items[2])
                }
                MSG_CREATE_OES_TEXTURE_SUCCESS -> {
                    mOnOESTextureListener?.onTextureCall(msg.arg1)
                }
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
        mOnDrawListener?.onDraw()
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


    /**====================  编码器  encoder 方法 **********************/

    /**
     * 1-ffmpeg 编码，2-硬编码
     */
    external fun native_init_muxer(type:Int)

    /***
     * 1,2,3,4，5  目前只在用 2-yuv420p数据，  5 -oes 扩展纹理不做处理
     *
     * */
    @Deprecated("用oes扩展纹理，这个不再使用")
    external fun native_onCameraFrameDataValible(type:Int,byteArray: ByteArray)

    // 1-开始录音
    external fun native_initEncode(path:String = getRamdowVideoPath())

    external fun native_startEncode()

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
    /***
     * 更新oes纹理matrix
     */
    external fun native_updateMatrix(floatMatrix: FloatArray)


    companion object{
        // communicate code
        private const val MSG_ERROR = 5 // 播放错误
        private const val MSG_MERGE_AV = 7 // 合并视频
        private const val MSG_CREATE_OES_TEXTURE_SUCCESS = 8 // 创建扩展纹理成功

        /**
         * 是播放器还是视频合成
         */
        private const val ROOT_TYPE_PLAY = 1
        private const val ROOT_TYPE_MUXER = 2

        init {
            System.loadLibrary("native-mediamuxer")
        }
    }
}
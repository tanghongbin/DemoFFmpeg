package com.example.democ.render

import android.opengl.GLSurfaceView
import android.os.Handler
import android.view.Surface
import com.example.democ.audio.log
import com.example.democ.interfaces.MsgCallback
import com.example.democ.opengles.NativeRender
import com.example.common_base.utils.Constants
import javax.microedition.khronos.egl.EGLConfig
import javax.microedition.khronos.opengles.GL10

class FFmpegRender : GLSurfaceView.Renderer{

    private var mCallBack:MsgCallback? = null
    private val mHandler = Handler()

    companion object{
        init {
            System.loadLibrary("native-ffmpegrender")
        }

        val IMAGE_FORMAT_RGBA = 0x01
        val IMAGE_FORMAT_NV21 = 0x02
        val IMAGE_FORMAT_NV12 = 0x03
        val IMAGE_FORMAT_I420 = 0x04
    }

    fun setMsgCallback(call:MsgCallback){
        mCallBack = call
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
        val url = Constants.MP4_PLAY_BIG_PATH
//        val url = MuxerManager.MP4_PLAY_PATH
        // 1-音屏（不用指定），2-视频，3-yuv视频-不设置，4-opengl渲染
//        playMP4(url,null,2)

    }

    // =============== native 回调部分  =====================

    private fun nativeMsgCallback(type: Int){
        log("java layer has received msg :${type}")
        NativeRender.log("java layer has received msg:${type} call:${mCallBack}")
        mHandler.post {
            mCallBack?.callback(type)
        }
    }
    
    private fun renderDimensionCallFromJni(renderWidth:Int,renderHeight:Int){
        mHandler.post {
            log("打印width:${renderWidth} height:${renderHeight}")
            mCallBack?.renderSizeDimensionChanged(renderWidth,renderHeight)
        }
    }
    private fun onDecodeReadyDuration(duration:Int){
        mHandler.post {
            mCallBack?.processRangeSetup(0,duration)
            log("时长：${duration}")
        }
    }

    // ==============   播放部分 ======================

    external fun native_seekPosition(position:Int)

    external fun getVideoDimensions():Array<Int>

    external fun native_OnSurfaceCreated()

    external fun native_OnSurfaceChanged(width:Int,height:Int)

    external fun native_OnDrawFrame()

    external fun playMP4(url:String, surface: Surface?)

//    external fun encodeAudio()
//
//    external fun encodeVideo()

    external fun encodeYuvToImage(type: String):String

    external fun native_audioTest(type: Int)

    external fun native_unInit()

    fun startEncodeAudio(suffix:String){
        native_startEncode(".${suffix}")
    }

    private external fun native_startEncode(suffix:String)

    external fun native_encodeFrame(byteArray: ByteArray)

    external fun native_videoEncodeInit()

    external fun native_videoEncodeUnInit()

    external fun native_testReadFile()

    external fun native_yuv2rgb()

    external fun native_changeOutputFormat(path:String):String

    external fun native_addFilterToYuv(inputFileName:String):String

    external fun splitAudioAndVideo(path:String)

    external fun native_muxerAudioAndVideo()

    external fun native_startrecordmp4()

    external fun native_stoprecordmp4()

    external fun native_unInitRecordMp4():String

    external fun native_encodeavmuxer_OnSurfaceCreated()

    external fun native_encodeavmuxer_OnSurfaceChanged(width:Int,height:Int)

    external fun native_encodeavmuxer_encodeFrame(byteArray: ByteArray)

    external fun native_getSimpleInfo():String



    //混合音视频
//    external fun muxAv()

    // =====================  live 直播部分 ======================

    external fun native_live_OnSurfaceCreated()

    external fun native_live__OnSurfaceChanged(width:Int,height:Int)

    external fun native_live_startpush()

    external fun native_live_stoppush()

    external fun native_live_onDestroy()

    external fun native_live_encodeFrame(byteArray: ByteArray)

    //========================    RTMP 推流 ====================

    external fun native_sendPacketData(byteArray: ByteArray,type:Int)

    external fun native_rtmp_init(url:String)

    external fun native_rtmp_destroy()

}
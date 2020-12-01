package com.example.democ.render

import android.graphics.BitmapFactory
import android.opengl.GLSurfaceView
import android.view.Surface
import com.example.democ.R
import com.example.democ.audio.MuxerManager
import com.example.democ.audio.log
import com.example.democ.getAppContext
import java.nio.ByteBuffer
import javax.microedition.khronos.egl.EGLConfig
import javax.microedition.khronos.opengles.GL10

class FFmpegRender : GLSurfaceView.Renderer{

    companion object{
        init {
            System.loadLibrary("native-ffmpegrender")
        }

        val IMAGE_FORMAT_RGBA = 0x01
        val IMAGE_FORMAT_NV21 = 0x02
        val IMAGE_FORMAT_NV12 = 0x03
        val IMAGE_FORMAT_I420 = 0x04
    }
    override fun onDrawFrame(gl: GL10?) {
        native_OnDrawFrame()
        log("java layer draw frame")
    }

    override fun onSurfaceChanged(gl: GL10?, width: Int, height: Int) {
        native_OnSurfaceChanged(width,height)
    }

    override fun onSurfaceCreated(gl: GL10?, config: EGLConfig?) {
        native_OnSurfaceCreated()
//        setImageByType(1)
        val url = MuxerManager.MP4_PLAY_BIG_PATH
//        val url = MuxerManager.MP4_PLAY_PATH
        // 1-音屏（不用指定），2-视频，3-yuv视频-不设置，4-opengl渲染
//        playMP4(url,null,2)

    }


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

    external fun native_startEncode()

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

    //混合音视频
//    external fun muxAv()

}
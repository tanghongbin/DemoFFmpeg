package com.example.democ

import android.content.Context
import android.content.Intent
import android.os.Build
import android.os.Bundle
import android.util.Log
import android.view.View
import androidx.annotation.RequiresApi
import androidx.appcompat.app.AppCompatActivity
import com.example.common_base.DemoApplication
import com.example.democ.audio.*
import com.example.democ.opengles.EmptyActivity
import com.example.common_base.utils.requestCustomPermissions
import kotlinx.android.synthetic.main.activity_main.*

import java.lang.Runnable


class MainActivity : AppCompatActivity(), View.OnClickListener {


    val recorder by lazy { CustomAudioCapturer() }
    val player by lazy { CustomAudioTrack() }
    val encoder by lazy { AudioEncoder() }
    val decoder by lazy { AudioDecoder() }
    val videoDecoder = VideoDecodeManager()
    val audioDecoderPlayer = AudioDecodeManager()

    private var mTestStart = false
    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        requestCustomPermissions {
            setContent()
        }
    }




    private fun setContent() {
        setContentView(R.layout.activity_main)
        start.setOnClickListener {
            requestCustomPermissions {
                val start = System.nanoTime()
                mTestStart = true
                MuxerManager.getInstance().init()
                recorder.mListener = object : ByteBufferListener{
                    override fun listener(buffer: ByteArray) {
                        encoder.encode(buffer)
                    }
                }
                recorder.startCapture()
//                encoder.open()
                Thread(getEncodeTask()).start()
//                VideoEncoder.startEncode()
            }
        }
        stop.setOnClickListener {
            mSurface.onDestroy()
            encoder.close()
            recorder.stopCapture()
//            VideoEncoder.stopEncode()
            mTestStart = false
        }

        nativeRecordStart.setOnClickListener(this)
        nativeRecordStop.setOnClickListener(this)
        nativePlayStart.setOnClickListener(this)
        nativePlayStop.setOnClickListener(this)
        mOpenGlEs.setOnClickListener(this)
    }

    val native = NativeAudioTester()

    override fun onClick(v: View?) {
        when(v?.id){
            R.id.nativeRecordStart -> {
                videoDecoder.init(mBottomSurface.holder.surface)
                videoDecoder.startDecode()
                audioDecoderPlayer.init()
                audioDecoderPlayer.startDecode()
//                requestPermissions {
//                    native.startAudio(true)
//                }
            }
            R.id.nativeRecordStop -> {
//                native.stopAudio()
                videoDecoder.stopDecode()
                audioDecoderPlayer.stopDecode()
            }
            R.id.nativePlayStart -> {
                requestCustomPermissions {
                    native.startAudio(false)
                }
            }
            R.id.nativePlayStop -> {
                native.stopAudio()
            }
            R.id.mOpenGlEs -> {
                startActivity(Intent(this,EmptyActivity::class.java))
            }
        }
    }



    fun getEncodeTask() = Runnable {
        while (mTestStart) {
//            encoder.retrieve()
        }
        encoder.close()
    }

    @RequiresApi(Build.VERSION_CODES.LOLLIPOP)
    fun getDecodeTask() = Runnable {
        while (mTestStart) {
            decoder.retrieve()
        }
        decoder.close()
    }








    fun println(str: String) {
        Log.d(TAG, str)
    }


    inner class CustomArray(val array: IntArray) {
        override fun toString(): String {
            return array.contentToString()
        }
    }

    companion object {
        // Used to load the 'native-lib' library on application startup.

        val TAG = "TAG"
        init {

//            System.loadLibrary("native-lib")
        }

        fun log(str: String) {
            Log.d(TAG, str)
        }
    }


}

fun getAppContext():Context{
    return DemoApplication.instance
}


package com.example.democ.audio

import android.media.AudioFormat
import android.media.AudioRecord
import android.media.MediaRecorder
import android.util.Log
import com.example.democ.hwencoder.AudioConfiguration
import com.example.democ.interfaces.CaptureDataListener
import com.example.democ.utils.AudioUtils
import com.example.democ.utils.LogUtils
import java.lang.Exception

class CustomAudioCapturer : CaptureDataListener<ByteArray> {

    private var isCaptureStarted = false

    private var mMinBufferSize:Int = 0
    private var mAudio: AudioRecord? = null
    var mThread:Thread? = null
    var mListener: ByteBufferListener? = null
    private var captureOwn = false
    private var audioByte:ByteArray? = null

    fun startCapture(isCaptureManully:Boolean = false):Boolean{
        if (isCaptureStarted){
            log("已经开始了")
            return false
        }
        captureOwn = isCaptureManully
        mAudio = AudioUtils.getAudioRecord(AudioConfiguration.createDefault())
        mAudio?.startRecording()
        if (!isCaptureManully){
            mThread = Thread(getTask())
            mThread?.start()
        }
        mMinBufferSize = AudioUtils.getRecordBufferSize(AudioConfiguration.createDefault())
        audioByte = ByteArray(mMinBufferSize);
        isCaptureStarted = true
        return true
    }


    fun stopCapture(){
        if (!isCaptureStarted){
            return
        }
        try {
            if (!captureOwn){
                mThread?.interrupt()
                mThread?.join(1000)
                mThread = null
            }
        }catch (e:Exception){
            e.printStackTrace()
        }
        if (mAudio?.state == AudioRecord.RECORDSTATE_RECORDING){
            mAudio?.stop()
        }
        mAudio?.release()
        isCaptureStarted = false
    }

    private fun getTask() = Runnable {
        while (!Thread.interrupted()){
            val result = mAudio?.read(audioByte,0,mMinBufferSize)
            when(result){
                AudioRecord.ERROR_INVALID_OPERATION -> log(
                    "error_invalid_operation"
                )
                AudioRecord.ERROR_BAD_VALUE -> log("error_bad_value")
                else -> {
//                    log("this is audio pcm")
                    audioByte?.let {
                        mListener?.listener(it)
                    }
                }
            }
        }
        log("audio capture has finished")
    }
    // sync catch audio

    fun captureAudioManully():ByteArray{
        val byteArray = ByteArray(mMinBufferSize)
        mAudio?.read(byteArray,0,mMinBufferSize)
        return byteArray
    }

    override fun capture(): ByteArray {
        return captureAudioManully()
    }
}

interface ByteBufferListener{
    fun listener(buffer:ByteArray)
}

fun log(string: String,tag:String = "DemoC"){
    LogUtils.log(string,tag)
}
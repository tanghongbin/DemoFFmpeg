package com.example.democ.audio

import android.media.AudioFormat
import android.media.AudioRecord
import android.media.MediaRecorder
import android.util.Log
import java.lang.Exception

class CustomAudioCapturer {

    private var isCaptureStarted = false

    private val DEFAULT_SOURCE = MediaRecorder.AudioSource.MIC
    private val DEFAULT_SAMPLE_RATE = 44100
    private val DEFAULT_CHANNEL_CONFIG = AudioFormat.CHANNEL_IN_MONO
    private val DEFAULT_AUDIO_FORMAT = AudioFormat.ENCODING_PCM_16BIT
    private var mMinBufferSize:Int = 0
    private var mAudio: AudioRecord? = null
    var mThread:Thread? = null
    var mListener: ByteBufferListener? = null

    fun startCapture():Boolean{
        if (isCaptureStarted){
            log("已经开始了")
            return false
        }
        mMinBufferSize = AudioRecord.getMinBufferSize(DEFAULT_SAMPLE_RATE,DEFAULT_CHANNEL_CONFIG,DEFAULT_AUDIO_FORMAT)
        mMinBufferSize = 4096
        if (mMinBufferSize == AudioRecord.ERROR_BAD_VALUE){
            log("invalid parameters")
            return false
        }
        log("audioRecord minsize --$mMinBufferSize")
        mAudio = AudioRecord(DEFAULT_SOURCE,DEFAULT_SAMPLE_RATE,DEFAULT_CHANNEL_CONFIG,DEFAULT_AUDIO_FORMAT,mMinBufferSize)
        if (mAudio?.state == AudioRecord.STATE_UNINITIALIZED){
            log("init error")
            return false
        }
        mAudio?.startRecording()
        mThread = Thread(getTask())
        mThread?.start()
        isCaptureStarted = true
        log("start capture success")
        return true
    }


    fun stopCapture(){
        if (!isCaptureStarted){
            log("capture had not started")
            return
        }

        try {
            mThread?.interrupt()
            mThread?.join(1000)
            mThread = null
        }catch (e:Exception){
            e.printStackTrace()
        }
        if (mAudio?.state == AudioRecord.RECORDSTATE_RECORDING){
            mAudio?.stop()
        }

        mAudio?.release()
        isCaptureStarted = false
        log("stop capture success")
    }

    private fun getTask() = Runnable {
        while (!Thread.interrupted()){
            val byteArray = ByteArray(mMinBufferSize)
            val result = mAudio?.read(byteArray,0,mMinBufferSize)
            when(result){
                AudioRecord.ERROR_INVALID_OPERATION -> log(
                    "error_invalid_operation"
                )
                AudioRecord.ERROR_BAD_VALUE -> log("error_bad_value")
                else -> mListener?.listener(byteArray)
            }
            log("has capture a frame audio")
        }
        log("audio capture has finished")
    }

}

interface ByteBufferListener{
    fun listener(buffer:ByteArray)
}

fun log(string: String,tag:String = "TAG"){
    Log.d(tag,string)
}
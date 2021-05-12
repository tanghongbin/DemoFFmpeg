package com.example.democ.audio

import android.media.AudioFormat
import android.media.AudioManager
import android.media.AudioTrack

class CustomAudioTrack {
    private var isCaptureStarted = false

    private val DEFAULT_STREAM_TYPE = AudioManager.STREAM_MUSIC
    private val DEFAULT_SAMPLE_RATE = 44100
    private val DEFAULT_CHANNEL_CONFIG = AudioFormat.CHANNEL_OUT_MONO
    private val DEFAULT_AUDIO_FORMAT = AudioFormat.ENCODING_PCM_16BIT
    private val DEFAULT_PLAY_MODE = AudioTrack.MODE_STREAM

    private var mMinBufferSize:Int = 0
    private var mAudioTrack:AudioTrack? = null

    fun startPlay():Boolean{
        if (isCaptureStarted){
            log("player has started")
            return false
        }
        mMinBufferSize = AudioTrack.getMinBufferSize(DEFAULT_SAMPLE_RATE,DEFAULT_CHANNEL_CONFIG,
            DEFAULT_AUDIO_FORMAT)
        if (mMinBufferSize == AudioTrack.ERROR_BAD_VALUE) {
            log("Invalid parameter !")
            return false
        }
        log("audioTrack minsize --$mMinBufferSize")
        mAudioTrack = AudioTrack(DEFAULT_STREAM_TYPE,DEFAULT_SAMPLE_RATE,DEFAULT_CHANNEL_CONFIG,
            DEFAULT_AUDIO_FORMAT,mMinBufferSize,DEFAULT_PLAY_MODE)
        if (mAudioTrack?.state == AudioTrack.STATE_UNINITIALIZED){
            log("player didn't init")
            return false
        }
        log("player prepare success")
        isCaptureStarted = true
        return true
    }


    fun stopPlay(){
        if (!isCaptureStarted){
            log("player hasn't started")
            return
        }
        if (mAudioTrack?.state == AudioTrack.PLAYSTATE_PLAYING){
            mAudioTrack?.stop()
        }
        mAudioTrack?.release()
        isCaptureStarted = false
    }


    fun play(byteArray: ByteArray,offsetData:Int,audioSize:Int){
        if (!isCaptureStarted){
            log("player hasn't started")
            return
        }

        if (mAudioTrack?.write(byteArray,offsetData,audioSize) != audioSize) {
            log("Could not write all the samples to the audio device !")
        }
        mAudioTrack?.play()

        log("OK, Played $audioSize bytes !")
    }



}
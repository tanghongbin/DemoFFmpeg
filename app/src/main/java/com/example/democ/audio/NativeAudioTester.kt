package com.example.democ.audio

class NativeAudioTester {

    private var isCapture = true

    fun startAudio(isCapture:Boolean){
        this.isCapture = isCapture
        Thread {
            if (isCapture){
                nativeStartCapture()
            } else {
                nativeStartPlay()
            }
        }.start()
    }

    fun stopAudio(){
        if (isCapture){
            nativeStopCapture()
        } else {
            nativeStopPlay()
        }
    }

    private external fun nativeStartCapture():Boolean

    private external fun nativeStopCapture():Boolean

    private external fun nativeStartPlay():Boolean

    private external fun nativeStopPlay():Boolean

}
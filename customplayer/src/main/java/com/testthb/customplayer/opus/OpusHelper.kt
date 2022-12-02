package com.testthb.customplayer.opus


/***
 * opus 编码与解码
 */
class OpusHelper {
    companion object {
        init {
            System.loadLibrary("native-opus")
        }
    }
    private var mNativeOpusHelper:Long = 0L

    fun initOpus(){
        native_initOpus(1)
    }

    fun startEncode(){
        native_startEncode()
    }

    fun stopEncode() {
        native_stopEncode()
    }

    fun startPlayOpus() {
        native_startPlayOpus()
    }

    fun playPcm() {
        native_playPcm()
    }


    fun destroy(){
        native_destroyOpus()
    }



    private external fun native_initOpus(type:Int)
    private external fun native_startEncode()
    private external fun native_stopEncode()
    private external fun native_startPlayOpus()
    private external fun native_destroyOpus()
    private external fun native_playPcm()
}
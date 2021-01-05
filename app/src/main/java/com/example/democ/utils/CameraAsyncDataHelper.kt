package com.example.democ.utils

import com.example.camera.listener.CameraYUVDataListener
import com.example.democ.audio.log
import java.util.concurrent.ArrayBlockingQueue
import java.util.concurrent.LinkedBlockingQueue

class CameraAsyncDataHelper {
    private var isStop = false
    private val linkedQueue = ArrayBlockingQueue<ByteArray>(10)
    var mListener: CameraYUVDataListener? = null
    private val thread = Thread {
        while (!isStop) {
            if (linkedQueue.size > 0){
                val result = linkedQueue.poll()
                log("data is null $result")
                result?.let {
                    mListener?.onCallback(it)
                }
            }
        }
    }

    fun addByteData(data: ByteArray?) {
        if (linkedQueue.size < 3)
            linkedQueue.add(data)
    }

    init {
        thread.start()
    }

    fun onDestroy() {
        isStop = true
    }
}
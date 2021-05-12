package com.example.democ.utils

import android.util.Log
import com.example.democ.audio.log
import kotlinx.coroutines.CoroutineScope
import kotlinx.coroutines.Dispatchers
import kotlinx.coroutines.Job
import kotlinx.coroutines.launch
import net.butterflytv.rtmp_client.RTMPMuxer

/**
 * Created by zhangxd on 2018/8/29.
 * rtmp 推流客户端
 */
class RtmpPushManager {
    private val mRtmpMuxer = RTMPMuxer()
    private val coroutineScope by lazy { CoroutineScope(Job()) }
    fun init(url: String?) {
        var url = url
        if (url == null || url == "") {
            url = URL
        }
        excute {
            val status = mRtmpMuxer.open(url, 1280, 720)
            log(" RtmpClientManager open status :$status url: $url")
            if (mRtmpMuxer.isConnected == 1) {
                log(" RtmpClientManager connected..")
//                mRtmpMuxer.write_flv_header(true,true)
            }
        }
        log("rtmp init over")
    }


    fun writeVideoData(data: ByteArray, timestamp: Int) {
        val result = mRtmpMuxer.writeVideo(data, 0, data.size, timestamp)
        log("write video result:${result}")
    }


    fun writeAudioData(data: ByteArray, timestamp: Int) {
        val result = mRtmpMuxer.writeAudio(data, 0, data.size, timestamp)
        log("write audio result:${result}")
    }

    fun close() {
        excute {
            mRtmpMuxer.close()
        }
    }

    companion object {
        private const val URL =
            "rtmp://106.13.78.235/demo/androidPush"
    }

    fun excute(block:() -> Unit){
        coroutineScope.launch(Dispatchers.Default) {
            block()
        }
    }
}
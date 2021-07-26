package com.example.customplayer.activity.ffmpeg

import android.os.Build
import android.os.Bundle
import androidx.annotation.RequiresApi
import androidx.appcompat.app.AppCompatActivity
import com.example.common_base.utils.log
import com.example.customplayer.R
import com.example.customplayer.player.CustomMediaController
import com.example.customplayer.util.webrtc.RtcConnectHelper
import java.sql.RowId
import java.util.*

/***
 * webrtc 实时通信
 */
class WebrtcActivity : AppCompatActivity(), RtcConnectHelper.RtcConnectListener {

    private val mConnectHelper by  lazy { RtcConnectHelper() }
    @RequiresApi(Build.VERSION_CODES.O)
    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        setContentView(R.layout.activity_webrtc)
        mConnectHelper.setConnectListener(this)
        mConnectHelper.joinRoom(getString(R.string.server2),UUID.randomUUID().toString(),"thb007")
    }


    override fun onConnecting() {
        log("i'm connecting")
    }

    override fun onConnected() {
        log("already connected")
    }

    override fun onDisconnect() {
        log("something is trouble,disconnected")
    }

    override fun onRemoteUserJoin(userId: String) {
        log("hi , there is a new guy here :${userId}")
    }

    override fun onRemoteUserLeft(userId: String) {
        log("shit,the guy gone:${userId}")
    }

    override fun onMessage() {

    }


    override fun onDestroy() {
        mConnectHelper.leftRoom()
        super.onDestroy()
    }
}
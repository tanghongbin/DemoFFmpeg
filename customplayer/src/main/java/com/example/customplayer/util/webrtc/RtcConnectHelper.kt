package com.example.customplayer.util.webrtc

import com.example.common_base.utils.log
import io.socket.client.IO
import io.socket.client.Socket
import org.json.JSONObject

class RtcConnectHelper {
    private var mSocket: Socket? = null
    private var mUserId:String = ""
    private var mRoomName = ""
    private var mListener:RtcConnectListener? = null

    companion object{
        const val REMOTE_USERJOIN = "user-joined"
        const val REMOTE_USERLEFT = "user-left"
        const val BROADCAST = "broadcast"
        const val JOIN_ROOM = "join-room"
        const val LEAVE_ROOM = "leave-room"

    }

    interface RtcConnectListener{
        fun onConnecting()
        fun onConnected()
        fun onDisconnect()
        fun onRemoteUserJoin(userId: String)
        fun onRemoteUserLeft(userId: String)
        fun onMessage()
    }

    fun setConnectListener(listener: RtcConnectListener){
        mListener = listener
    }

    /**
     * 加入房间
     */
    fun joinRoom(url:String,userId:String,roomName:String){
        mSocket = IO.socket(url)
        mSocket?.connect()
        mUserId = userId
        mRoomName = roomName
        listenEvents()
    }

    /**
     *  监听事件
     */
    private fun listenEvents() {
        if (mSocket == null) return
        mSocket!!.on(Socket.EVENT_CONNECT_ERROR) {
            log("connect error :${it.toString()}")
        }
        mSocket!!.on(Socket.EVENT_ERROR) {
            log("error:${it}")
        }
        mSocket!!.on(Socket.EVENT_CONNECT) {
            sendMessage(JOIN_ROOM,JSONObject().append("userId",mUserId).append("roomName",mRoomName))
            mListener?.onConnected()
        }
        mSocket!!.on(Socket.EVENT_CONNECTING) {
            mListener?.onConnecting()
        }
        mSocket!!.on(Socket.EVENT_DISCONNECT) {
            mListener?.onDisconnect()
        }
        mSocket!!.on(Socket.EVENT_CONNECT_TIMEOUT) {
            log("connect timeout, please check again")
        }

        mSocket!!.on(REMOTE_USERJOIN) {
            val userId = it[0].toString()
            if (userId == mUserId || mListener == null) return@on
            mListener?.onRemoteUserJoin(it[0].toString())
        }
        mSocket!!.on(REMOTE_USERLEFT) {
            val userId = it[0].toString()
            if (userId == mUserId || mListener == null) return@on
            mListener?.onRemoteUserLeft(it[0].toString())
        }
        mSocket!!.on(BROADCAST) {
            mListener?.onMessage()
        }

    }

    fun sendMessage(eventName:String,jsonObject: JSONObject){
        if (mSocket == null) return
        mSocket!!.emit(eventName,jsonObject)
    }

    /**
     * 离开房间
     */
    fun leftRoom(){
        if (mSocket == null) return
        mSocket!!.emit(LEAVE_ROOM,JSONObject().append("userId",mUserId).append("roomName",mRoomName))
        mSocket!!.close()
        mSocket = null
    }



}

fun JSONObject.append(key:String,value:Any):JSONObject{
    put(key,value)
    return this
}
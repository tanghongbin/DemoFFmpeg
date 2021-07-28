package com.example.customplayer.util.webrtc

import com.example.avutils.utils.runCatchException
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

        const val MESSAGE_TYPE_OFFER = 0x01
        const val MESSAGE_TYPE_ANSWER = 0x02
        const val MESSAGE_TYPE_CANDIDATE = 0x03
        const val MESSAGE_TYPE_HANGUP = 0x04
    }

    fun getUserId():String{
        return mUserId
    }

    interface RtcConnectListener{
        fun onConnecting()
        fun onConnected()
        fun onDisconnect()
        fun onRemoteUserJoin(userId: String)
        fun onRemoteUserLeft(userId: String)
        fun onMessage(json: JSONObject)
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
            sendCmdMessage(JOIN_ROOM,JSONObject().appendValue("userId",mUserId).appendValue("roomName",mRoomName))
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
            runCatchException {
                val userId = it[0].toString()
                if (userId == mUserId || mListener == null) return@runCatchException
                mListener?.onRemoteUserJoin(it[0].toString())
            }
        }
        mSocket!!.on(REMOTE_USERLEFT) {
            runCatchException {
                val userId = it[0].toString()
                if (userId == mUserId || mListener == null) return@runCatchException
                mListener?.onRemoteUserLeft(it[0].toString())
            }
        }
        mSocket!!.on(BROADCAST) {
            runCatchException {
                val json =it[0] as  JSONObject
                val userId = json.getString("userId")
                if (userId != mUserId)  mListener?.onMessage(json)
            }
        }

    }

    fun sendCmdMessage(eventName:String, jsonObject: JSONObject){
        if (mSocket == null) return
        mSocket!!.emit(eventName,jsonObject.toString())
    }

    fun sendBrocastMessage(jsonObject: JSONObject){
        mSocket!!.emit(BROADCAST,jsonObject)
    }

    /**
     * 离开房间
     */
    fun leftRoom(){
        if (mSocket == null) return
        sendCmdMessage(LEAVE_ROOM,JSONObject().appendValue("userId",mUserId).appendValue("roomName",mRoomName))
        mSocket!!.close()
        mSocket = null
    }



}

fun JSONObject.appendValue(key:String,value:String?):JSONObject{
    put(key,value)
    return this
}
fun JSONObject.appendValue(key:String,value:Int?):JSONObject{
    put(key,value)
    return this
}
fun JSONObject.appendValue(key:String,value:Long?):JSONObject{
    put(key,value)
    return this
}
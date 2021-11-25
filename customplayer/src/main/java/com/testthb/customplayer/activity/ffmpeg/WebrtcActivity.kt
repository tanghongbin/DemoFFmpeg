package com.testthb.customplayer.activity.ffmpeg

import android.content.Context
import android.os.Build
import android.os.Bundle
import androidx.annotation.RequiresApi
import androidx.appcompat.app.AppCompatActivity
import com.testthb.avutils.utils.runCatchException
import com.testthb.common_base.utils.log
import com.testthb.customplayer.R
import com.testthb.customplayer.util.webrtc.RtcConnectHelper
import com.testthb.customplayer.util.webrtc.appendValue
import kotlinx.android.synthetic.main.activity_webrtc.*
import org.json.JSONException
import org.json.JSONObject
import org.webrtc.*

/***
 * webrtc 实时通信
 */
class WebrtcActivity : AppCompatActivity(), RtcConnectHelper.RtcConnectListener ,
PeerConnection.Observer{

    private lateinit var mAudioTrack: AudioTrack
    private lateinit var mVideoTrack: VideoTrack
    private lateinit var mSurfaceTextureHelper: SurfaceTextureHelper
    private var mVideoCapturer: VideoCapturer? = null
    private lateinit var mPeerConnectionFactory: PeerConnectionFactory
    private lateinit var mRootEglBase: EglBase
    private val USER_ID = "thb001${Build.DEVICE}"
    private val mConnectionMap = HashMap<String?,PeerConnection?>()
    private var mRemoteCount = 0

    private val mConnectHelper by  lazy { RtcConnectHelper() }

    val VIDEO_TRACK_ID = "ARDAMSv0"
    val AUDIO_TRACK_ID = "ARDAMSa0"
    private val VIDEO_RESOLUTION_WIDTH = 1280
    private val VIDEO_RESOLUTION_HEIGHT = 720
    private val VIDEO_FPS = 30

    @RequiresApi(Build.VERSION_CODES.O)
    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        setContentView(R.layout.activity_webrtc)
        mConnectHelper.setConnectListener(this)
        mConnectHelper.joinRoom(getString(R.string.server2),USER_ID,"thb007")
        setUpViews()
        mWebrtcCall.setOnClickListener {
            startCall()
        }
    }

    private fun startCall() {
        forEachAllUser {userId ->
            var mPeerConnection = mConnectionMap[userId]
            if (mPeerConnection == null){
                mPeerConnection = createConnection()
            }
            val constraints = MediaConstraints()
            constraints.mandatory.add(MediaConstraints.KeyValuePair("OfferToReceiveAudio","true"))
            constraints.mandatory.add(MediaConstraints.KeyValuePair("OfferToReceiveVideo","true"))
            constraints.mandatory.add(MediaConstraints.KeyValuePair("DtlsSrtpKeyAgreement","true"))
            mPeerConnection?.createOffer(object : SimpleSdpObserver() {
                override fun onCreateSuccess(sessionDescription: SessionDescription) {
                    log("createOffer onCreateSuccess")
                    mPeerConnection.setLocalDescription(SimpleSdpObserver(),sessionDescription)
                    val json = JSONObject().appendValue("userId",mConnectHelper.getUserId())
                            .appendValue("msgType",RtcConnectHelper.MESSAGE_TYPE_OFFER)
                            .appendValue("sdp",sessionDescription.description)
                    mConnectHelper.sendBrocastMessage(json)
                }
            },constraints)
            mConnectionMap[userId] = mPeerConnection
            log("startCall")
        }
    }

    private fun createConnection(): PeerConnection? {
        val configration =  PeerConnection.RTCConfiguration(ArrayList())
        val connection = mPeerConnectionFactory.createPeerConnection(configration,this)
        if (connection == null) {
            log("failed to create connection")
            return null
        }
        connection.addTrack(mAudioTrack)
        connection.addTrack(mVideoTrack)
        return connection
    }

    override fun onResume() {
        super.onResume()
        mVideoCapturer!!.startCapture(VIDEO_RESOLUTION_WIDTH, VIDEO_RESOLUTION_HEIGHT, VIDEO_FPS)
    }

    override fun onPause() {
        super.onPause()
        try {
            mVideoCapturer!!.stopCapture()
        } catch (e: InterruptedException) {
            e.printStackTrace()
        }
    }

    override fun onDestroy() {
        super.onDestroy()
        doEndCall()
        mLocalSurfaceView.release()
        mRemoteSurfaceView.release()
        mRemoteSurfaceView2.release()
        mVideoCapturer!!.dispose()
        mSurfaceTextureHelper.dispose()
        PeerConnectionFactory.stopInternalTracingCapture()
        PeerConnectionFactory.shutdownInternalTracer()
        mConnectHelper.leftRoom()
    }


    private fun doEndCall() {
        log("doEndCall")
        hanup()
        val message = JSONObject()
        try {
            message.put("userId", mConnectHelper.getUserId())
            message.put("msgType", RtcConnectHelper.MESSAGE_TYPE_HANGUP)
            mConnectHelper.sendBrocastMessage(message)
        } catch (e: JSONException) {
            e.printStackTrace()
        }
    }

    private fun hanup() {
        forEachAllUser {
            val mPeerConnection = mConnectionMap[it]
            if (mPeerConnection != null) {
                mPeerConnection.close()
                mConnectionMap.remove(it)
            }
        }

//        updateCallState(true)
    }

    fun forEachAllUser(block:(String?) -> Unit){
        mConnectionMap.keys.forEach {
            block(it)
        }
    }


    private fun setUpViews() {
        mRootEglBase = EglBase.create()
        mLocalSurfaceView.init(mRootEglBase.eglBaseContext, null)
        mLocalSurfaceView.setScalingType(RendererCommon.ScalingType.SCALE_ASPECT_FILL)
        mLocalSurfaceView.setMirror(true)
        mLocalSurfaceView.setEnableHardwareScaler(false /* enabled */)

        mRemoteSurfaceView.init(mRootEglBase.eglBaseContext, null)
        mRemoteSurfaceView.setScalingType(RendererCommon.ScalingType.SCALE_ASPECT_FILL)
        mRemoteSurfaceView.setMirror(true)
        mRemoteSurfaceView.setEnableHardwareScaler(true /* enabled */)
        mRemoteSurfaceView.setZOrderMediaOverlay(true)

        mRemoteSurfaceView2.init(mRootEglBase.eglBaseContext, null)
        mRemoteSurfaceView2.setScalingType(RendererCommon.ScalingType.SCALE_ASPECT_FILL)
        mRemoteSurfaceView2.setMirror(true)
        mRemoteSurfaceView2.setEnableHardwareScaler(true /* enabled */)
        mRemoteSurfaceView2.setZOrderMediaOverlay(true)

        val videoSink = ProxyVideoSink()
        videoSink.setTarget(mLocalSurfaceView)


        mPeerConnectionFactory = createPeerConnectionFactory(this)

        // NOTE: this _must_ happen while PeerConnectionFactory is alive!

        // NOTE: this _must_ happen while PeerConnectionFactory is alive!
        Logging.enableLogToDebugOutput(Logging.Severity.LS_VERBOSE)

        mVideoCapturer = createVideoCapturer()

        mSurfaceTextureHelper = SurfaceTextureHelper.create("CaptureThread", mRootEglBase.eglBaseContext)
        val videoSource = mPeerConnectionFactory.createVideoSource(false)
        mVideoCapturer!!.initialize(mSurfaceTextureHelper, applicationContext, videoSource.capturerObserver)


        mVideoTrack = mPeerConnectionFactory.createVideoTrack(VIDEO_TRACK_ID, videoSource)
        mVideoTrack.setEnabled(true)
        mVideoTrack.addSink(videoSink)

        val audioSource = mPeerConnectionFactory.createAudioSource(MediaConstraints())
        mAudioTrack = mPeerConnectionFactory.createAudioTrack(AUDIO_TRACK_ID, audioSource)
        mAudioTrack.setEnabled(true)

    }


    private fun createVideoCapturer(): VideoCapturer? {
        return if (Camera2Enumerator.isSupported(this)) {
            createCameraCapturer(Camera2Enumerator(this))
        } else {
            createCameraCapturer(Camera1Enumerator(true))
        }
    }


    private fun createCameraCapturer(enumerator: CameraEnumerator): VideoCapturer? {
        val deviceNames = enumerator.deviceNames

        // First, try to find front facing camera
        log( "Looking for front facing cameras.")
        for (deviceName in deviceNames) {
            if (enumerator.isFrontFacing(deviceName)) {
                log( "Creating front facing camera capturer.")
                val videoCapturer: VideoCapturer? = enumerator.createCapturer(deviceName, null)
                if (videoCapturer != null) {
                    return videoCapturer
                }
            }
        }

        // Front facing camera not found, try something else
        log( "Looking for other cameras.")
        for (deviceName in deviceNames) {
            if (!enumerator.isFrontFacing(deviceName)) {
                log( "Creating other camera capturer.")
                val videoCapturer: VideoCapturer? = enumerator.createCapturer(deviceName, null)
                if (videoCapturer != null) {
                    return videoCapturer
                }
            }
        }
        return null
    }

    private fun createPeerConnectionFactory(context: Context?): PeerConnectionFactory {
        val encoderFactory: VideoEncoderFactory
        val decoderFactory: VideoDecoderFactory
        encoderFactory = DefaultVideoEncoderFactory(
                mRootEglBase.eglBaseContext, false /* enableIntelVp8Encoder */, true)
        decoderFactory = DefaultVideoDecoderFactory(mRootEglBase.eglBaseContext)
        PeerConnectionFactory.initialize(PeerConnectionFactory.InitializationOptions.builder(context)
                .setEnableInternalTracer(true)
                .createInitializationOptions())
        val builder = PeerConnectionFactory.builder()
                .setVideoEncoderFactory(encoderFactory)
                .setVideoDecoderFactory(decoderFactory)
        builder.setOptions(null)
        return builder.createPeerConnectionFactory()
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
        if (!mConnectionMap.containsKey(userId)){
            mConnectionMap[userId] = null
        }
    }

    override fun onRemoteUserLeft(userId: String) {
        log("shit,the guy gone:${userId}")
        if (mConnectionMap.containsKey(userId) && mConnectionMap[userId] != null){
            val connnection = mConnectionMap[userId]
            connnection?.close()
            mConnectionMap.remove(userId)
        }
    }

    override fun onMessage(json: JSONObject) {
        val userId = json.getString("userId")
        when(json.getInt("msgType")){
            RtcConnectHelper.MESSAGE_TYPE_OFFER -> {
                onRemoteOfferReceived(userId,json)
            }
            RtcConnectHelper.MESSAGE_TYPE_ANSWER -> {
                onRemoteAnswerReceived(userId,json)
            }
            RtcConnectHelper.MESSAGE_TYPE_CANDIDATE -> {
                onRemoteCandidateReceived(userId,json)
            }
            RtcConnectHelper.MESSAGE_TYPE_HANGUP -> {
                hanup()
            }
        }
    }

    private fun onRemoteCandidateReceived(userId: String?, json: JSONObject) {
        log("onRemoteCandidateReceived userId:${userId}")
        runCatchException {
            val remoteCandidate = IceCandidate(json.getString("id"),json.getInt("label"),
                    json.getString("candidate"))
            mConnectionMap[userId]?.addIceCandidate(remoteCandidate)
        }
    }

    private fun onRemoteAnswerReceived(userId: String?, json: JSONObject) {
        log("onRemoteAnswerReceived userId:${userId}")
        runCatchException {
            val desc = json.getString("sdp")
            mConnectionMap[userId]?.setRemoteDescription(SimpleSdpObserver(),SessionDescription(SessionDescription.Type.ANSWER,desc))
        }
    }

    /**
     * 收到远程的呼叫
     */
    private fun onRemoteOfferReceived(userId: String?, json: JSONObject) {
        log("onRemoteAnswerReceived userId:${userId}")
        runCatchException {
            var mPeerConnection = mConnectionMap[userId]
            if (mPeerConnection == null){
                mPeerConnection = createConnection()
                mConnectionMap[userId] = mPeerConnection
            }
            val desc = json.getString("sdp")
            mPeerConnection?.setRemoteDescription(SimpleSdpObserver(),SessionDescription(SessionDescription.Type.OFFER,desc))
            doAnswerCall(userId)
        }
    }

    /***
     * 答复
     */
    private fun doAnswerCall(userId: String?) {
        val constraints = MediaConstraints()
        val mPeerConnection = mConnectionMap[userId]
        mPeerConnection?.createAnswer(object : SimpleSdpObserver(){
            override fun onCreateSuccess(sessionDescription: SessionDescription) {
                mPeerConnection.setLocalDescription(SimpleSdpObserver(),sessionDescription)
                val json = JSONObject().appendValue("userId",mConnectHelper.getUserId())
                        .appendValue("msgType",RtcConnectHelper.MESSAGE_TYPE_ANSWER)
                        .appendValue("sdp",sessionDescription.description)
                mConnectHelper.sendBrocastMessage(json)
            }
        },constraints)
    }

    override fun onIceCandidate(ice: IceCandidate?) {
        log("onIceCandidate userId:${ice}")
        val json = JSONObject().appendValue("userId",mConnectHelper.getUserId())
                .appendValue("msgType",RtcConnectHelper.MESSAGE_TYPE_CANDIDATE)
                .appendValue("label", ice?.sdpMLineIndex)
                .appendValue("id", ice?.sdpMid)
                .appendValue("candidate", ice?.sdp)
        mConnectHelper.sendBrocastMessage(json)
    }


    override fun onIceCandidatesRemoved(iceCandidates: Array<out IceCandidate>?) {
        mConnectionMap.values.forEach {connection ->
            connection?.removeIceCandidates(iceCandidates)
        }
    }


    override fun onDataChannel(p0: DataChannel?) {

    }

    override fun onIceConnectionReceivingChange(p0: Boolean) {

    }

    override fun onIceConnectionChange(p0: PeerConnection.IceConnectionState?) {

    }

    override fun onIceGatheringChange(p0: PeerConnection.IceGatheringState?) {

    }

    override fun onAddStream(p0: MediaStream?) {

    }

    override fun onSignalingChange(p0: PeerConnection.SignalingState?) {

    }

    override fun onRemoveStream(p0: MediaStream?) {

    }

    override fun onRenegotiationNeeded() {

    }

    override fun onAddTrack(receiver: RtpReceiver?, p1: Array<out MediaStream>?) {
        log("onAddTrack")
        val track = receiver?.track()
        if (track is VideoTrack){
            track.setEnabled(true)
            val sink = ProxyVideoSink()
            if (mRemoteCount == 0) {
                sink.setTarget(mRemoteSurfaceView)
            } else if (mRemoteCount == 1) {
                sink.setTarget(mRemoteSurfaceView2)
            }
            mRemoteCount++
            track.addSink(sink)
        }
    }


}

class ProxyVideoSink : VideoSink {
    private var mTarget: VideoSink? = null

    @Synchronized
    override fun onFrame(frame: VideoFrame) {
        if (mTarget == null) {
            log("Dropping frame in proxy because target is null.")
            return
        }
        mTarget!!.onFrame(frame)
    }

    @Synchronized
    fun setTarget(target: VideoSink?) {
        mTarget = target
    }

}

open class SimpleSdpObserver : SdpObserver {
    override fun onCreateSuccess(sessionDescription: SessionDescription) {
        log( "SdpObserver: onCreateSuccess !")
    }

    override fun onSetSuccess() {
        log( "SdpObserver: onSetSuccess")
    }

    override fun onCreateFailure(msg: String) {
        log( "SdpObserver onCreateFailure: $msg")
    }

    override fun onSetFailure(msg: String) {
        log( "SdpObserver onSetFailure: $msg")
    }
}
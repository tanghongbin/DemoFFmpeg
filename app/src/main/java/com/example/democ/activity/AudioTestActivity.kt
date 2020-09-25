package com.example.democ.activity

import androidx.appcompat.app.AppCompatActivity
import android.os.Bundle
import android.view.View
import com.example.democ.R
import com.example.democ.audio.*
import kotlinx.android.synthetic.main.activity_audio_test.*
import java.util.concurrent.locks.ReentrantLock

class AudioTestActivity : AppCompatActivity() ,View.OnClickListener{

    private lateinit var mAudioCapture:AudioCapturer
    private lateinit var mAudioPlayer:AudioPlayer

    private lateinit var mAudioEncoder:AudioEncoder
    private lateinit var mAudioDecoder:AudioDecoder

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        setContentView(R.layout.activity_audio_test)
        mStart.setOnClickListener(this)
        mEnd.setOnClickListener(this)
        mAudioCapture = AudioCapturer()
        mAudioPlayer = AudioPlayer()
        mAudioEncoder = AudioEncoder()
        mAudioDecoder = AudioDecoder()

    }

    override fun onClick(v: View?) {
        when(v?.id){
            R.id.mStart -> {
                mAudioCapture.setOnAudioFrameCapturedListener {
                    log("capture success data:${it}")
                    mAudioEncoder.encode(it,System.nanoTime()/1000)
                }
                mAudioEncoder.setAudioEncodedListener { encoded, presentationTimeUs ->
                    log("encode success data:${encoded}")
                    mAudioDecoder.decode(encoded,presentationTimeUs)
                }
                mAudioDecoder.setAudioDecodedListener { decoded, _ ->
                    log("decode success data:${decoded}")
                    mAudioPlayer.play(decoded,0,decoded.size)
                }
                mAudioEncoder.open()
                mAudioDecoder.open()
                mAudioEncoder.startRunTask()
                mAudioDecoder.startRunTask()
                mAudioCapture.startCapture()
                mAudioPlayer.startPlayer()
            }
            R.id.mEnd -> {
                mAudioPlayer.stopPlayer()
                mAudioCapture.stopCapture()
                mAudioEncoder.close()
                mAudioDecoder.close()
            }
        }
    }
}

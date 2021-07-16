package com.example.democ.activity

import androidx.appcompat.app.AppCompatActivity
import android.os.Bundle
import android.view.View
import com.example.democ.R
import com.example.democ.audio.*
import kotlinx.android.synthetic.main.activity_audio_test.*

class AudioTestActivity : AppCompatActivity() ,View.OnClickListener{

    private lateinit var mAudioCapture: AudioRecorder
    private lateinit var mAudioPlayer:AudioPlayer

    private lateinit var mAudioEncoder:AudioEncoder
    private lateinit var mAudioDecoder:AudioDecoder

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        setContentView(R.layout.activity_audio_test)
        mStart.setOnClickListener(this)
        mEnd.setOnClickListener(this)
        mAudioCapture = AudioRecorder()
        mAudioPlayer = AudioPlayer()
        mAudioEncoder = AudioEncoder()
        mAudioDecoder = AudioDecoder()

    }

    override fun onClick(v: View?) {
        when(v?.id){
            R.id.mStart -> {
                mAudioCapture.setOnAudioFrameCapturedListener { data,ret ->
                    log("capture success data:${data}")
                    mAudioEncoder.encode(data)
                }
                mAudioEncoder.setAudioEncodedListener { encoded, presentationTimeUs ->
                    log("encode success data:${encoded}")
                    mAudioDecoder.decode(encoded,presentationTimeUs)
                }
                mAudioDecoder.setAudioDecodedListener { decoded, _ ->
                    log("decode success data:${decoded}")
                    mAudioPlayer.play(decoded,0,decoded.size)
                }
//                mAudioEncoder.open()
                mAudioDecoder.open()
                mAudioEncoder.startEncode()
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

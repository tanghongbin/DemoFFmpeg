package com.example.democ.ffmpeg

import android.hardware.Camera
import android.os.Bundle
import android.view.SurfaceHolder
import androidx.appcompat.app.AppCompatActivity
import com.example.democ.R
import com.example.democ.audio.log
import com.example.democ.getBitmapFromYuvdata
import com.example.democ.render.FFmpegRender
import com.example.democ.runAsyncTask
import com.example.democ.utils.CameraHelper
import kotlinx.android.synthetic.main.activity_f_fmpeg_encode_video.*


class FFmpegEncodeVideoActivity : AppCompatActivity(), Camera.PreviewCallback,
    SurfaceHolder.Callback {

    lateinit var mRender:FFmpegRender
    lateinit var mCameraHelper: CameraHelper
    lateinit var camera: android.hardware.Camera
    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        setContentView(R.layout.activity_f_fmpeg_encode_video)
        mCameraHelper = CameraHelper(this,mSurface,this)
        mCameraHelper.init(1920,1080)
        mCameraHelper.setPreviewCallback(this)
        mRender = FFmpegRender()
        mReadFile.setOnClickListener {
            mRender.native_testReadFile()
        }
    }

    var isSet = false



    override fun onPreviewFrame(data: ByteArray?, camera: Camera?) {
        if (!isSet){
            isSet = true
            mImage.setImageBitmap(getBitmapFromYuvdata(data))
        }
        data?.apply {
            log("打印数组大小:${data.size}")
            mRender.native_encodeFrame(data)
        }

    }


    override fun onBackPressed() {

        super.onBackPressed()
    }

    override fun onDestroy() {
        super.onDestroy()
    }

    override fun surfaceChanged(holder: SurfaceHolder?, format: Int, width: Int, height: Int) {
        mRender.native_OnSurfaceChanged(mCameraHelper.getVideoWidth(),mCameraHelper.getVideoHeight())
    }

    override fun surfaceDestroyed(holder: SurfaceHolder?) {
//        mCameraHelper.release()
        mRender.native_videoEncodeUnInit()
    }

    override fun surfaceCreated(holder: SurfaceHolder?) {
        runAsyncTask({
            mRender.native_videoEncodeInit()
        },{

        })

    }
}

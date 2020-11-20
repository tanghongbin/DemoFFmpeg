package com.example.democ.ffmpeg

import android.app.Activity
import android.graphics.ImageFormat
import android.graphics.Point
import android.hardware.Camera
import android.hardware.Camera.CameraInfo
import android.os.Bundle
import android.util.DisplayMetrics
import android.view.Surface
import android.view.SurfaceHolder
import android.view.WindowManager
import androidx.appcompat.app.AppCompatActivity
import com.example.democ.R
import com.example.democ.audio.log
import com.example.democ.render.FFmpegRender
import com.example.democ.runAsyncTask
import com.example.democ.utils.CameraHelper
import kotlinx.android.synthetic.main.activity_f_fmpeg_encode_video.*
import java.util.concurrent.CopyOnWriteArrayList


class FFmpegEncodeVideoActivity : AppCompatActivity(), Camera.PreviewCallback,
    SurfaceHolder.Callback {

    lateinit var mRender:FFmpegRender
    lateinit var mCameraHelper: CameraHelper
    lateinit var camera: android.hardware.Camera
    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        setContentView(R.layout.activity_f_fmpeg_encode_video)
        mCameraHelper = CameraHelper(this,mSurface,this)
        mCameraHelper.init()
        mCameraHelper.setPreviewCallback(this)
        mRender = FFmpegRender()
        mReadFile.setOnClickListener {
            mRender.native_testReadFile()
        }
    }



    override fun onPreviewFrame(data: ByteArray?, camera: Camera?) {
        data?.apply {
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
        mRender.native_OnSurfaceChanged(width,height)
    }

    override fun surfaceDestroyed(holder: SurfaceHolder?) {
        mCameraHelper.release()
        mRender.native_videoEncodeUnInit()
    }

    override fun surfaceCreated(holder: SurfaceHolder?) {
        runAsyncTask({
            mRender.native_videoEncodeInit()
        },{

        })

    }
}

package com.example.democ.ffmpeg

import android.app.ActivityManager
import android.content.Context
import android.content.pm.ConfigurationInfo
import android.hardware.Camera
import android.opengl.GLSurfaceView
import android.os.Bundle
import android.view.SurfaceHolder
import androidx.appcompat.app.AppCompatActivity
import com.example.democ.R
import com.example.democ.audio.log
import com.example.democ.getBitmapFromYuvdata
import com.example.democ.render.FFmpegOpenGLRender
import com.example.democ.utils.CameraHelper
import kotlinx.android.synthetic.main.activity_f_fmpeg_encode_video.*
import kotlinx.android.synthetic.main.activity_f_fmpeg_open_g_l.*
import kotlinx.android.synthetic.main.activity_f_fmpeg_open_g_l.mSurface

class FFmpegTextureActivity : AppCompatActivity(), SurfaceHolder.Callback, Camera.PreviewCallback {

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        setContentView(R.layout.activity_f_fmpeg_open_g_l)
        init()
    }

    var isSet = false

    private fun init() {
//        mOpenGlEs.init(FFmpegOpenGLRender())

        var mCameraHelper = CameraHelper(this, mSurface, this)
        mCameraHelper.init(1920,1080)
        mCameraHelper.setPreviewCallback(this)
    }

    override fun surfaceChanged(holder: SurfaceHolder?, format: Int, width: Int, height: Int) {

    }

    override fun surfaceDestroyed(holder: SurfaceHolder?) {

    }

    override fun surfaceCreated(holder: SurfaceHolder?) {

    }

    override fun onPreviewFrame(data: ByteArray?, camera: Camera?) {
       if (!isSet){
           isSet = true
           mGlImage.setImageBitmap(getBitmapFromYuvdata(data))
       }
    }
}

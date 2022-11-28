package com.testthb.customplayer.activity.cameralearn

import android.graphics.SurfaceTexture
import android.media.MediaCodec
import android.media.MediaFormat
import androidx.appcompat.app.AppCompatActivity
import android.os.Bundle
import androidx.camera.core.CameraSelector
import androidx.camera.core.ImageCapture
import androidx.camera.core.Preview
import androidx.camera.lifecycle.ProcessCameraProvider
import androidx.core.content.ContextCompat
import com.ruiyi.mvvmlib.permission.EasyPermission
import com.testthb.customplayer.R
import kotlinx.android.synthetic.main.activity_camera_learn.*
import kotlinx.coroutines.Dispatchers

class CameraLearnActivity : AppCompatActivity() {
    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        setContentView(R.layout.activity_camera_learn)
        EasyPermission.init(this)
            .permissions(android.Manifest.permission.CAMERA,
            android.Manifest.permission.RECORD_AUDIO,
            android.Manifest.permission.WRITE_EXTERNAL_STORAGE)
            .request { allGranted, grantedList, deniedList ->
                startCamera()
            }
        takePic.setOnClickListener {
            takePhoto()
        }
    }

    private fun takePhoto() {

    }

    private fun startCamera() {
        val cameraProviderFuture = ProcessCameraProvider.getInstance(this)
        cameraProviderFuture.addListener({
            val cameraProvider = cameraProviderFuture.get()
            val preview = Preview.Builder().build().apply {
                setSurfaceProvider(viewFinder.surfaceProvider)
            }
            val capture = ImageCapture.Builder().build()
            val cameraSelector = CameraSelector.DEFAULT_FRONT_CAMERA
            cameraProvider.unbindAll()
            cameraProvider.bindToLifecycle(this,cameraSelector,preview,capture)
        },ContextCompat.getMainExecutor(this))
    }
}
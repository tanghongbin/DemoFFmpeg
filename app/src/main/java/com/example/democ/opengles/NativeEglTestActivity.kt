package com.example.democ.opengles

import android.graphics.Bitmap
import android.graphics.BitmapFactory
import android.opengl.GLES20
import android.opengl.GLException
import android.os.Bundle
import androidx.appcompat.app.AppCompatActivity
import com.example.democ.R
import kotlinx.android.synthetic.main.activity_native_egl_test.*
import java.io.IOException
import java.io.InputStream
import java.nio.ByteBuffer
import java.nio.IntBuffer


@Deprecated("")
class NativeEglTestActivity : AppCompatActivity() {

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        setContentView(R.layout.activity_native_egl_test)

        val mBgRender = NativeRender()
// 初始化创建 GLES 环境
        // 初始化创建 GLES 环境
        mBgRender.native_eglInit()
// 加载图片数据到纹理
        // 加载图片数据到纹理
        loadRGBAImage(R.mipmap.test2, mBgRender)
// 离屏渲染
        // 离屏渲染
        mBgRender.native_eglDraw()
// 从缓冲区读出渲染后的图像数据，加载到 ImageView
        // 从缓冲区读出渲染后的图像数据，加载到 ImageView
        mImage.setImageBitmap(createBitmapFromGLSurface(0, 0, 1080, 1664))
// 释放 GLES 环境
        // 释放 GLES 环境
        mBgRender.native_eglUnInit()

        mText.text =  mBgRender.getAvCodeInfo()
    }

    private fun loadRGBAImage(resId: Int, render: NativeRender) {
        val `is`: InputStream = this.resources.openRawResource(resId)
        val bitmap: Bitmap?
        try {
            bitmap = BitmapFactory.decodeStream(`is`)
            if (bitmap != null) {
                val bytes = bitmap.byteCount
                val buf: ByteBuffer = ByteBuffer.allocate(bytes)
                bitmap.copyPixelsToBuffer(buf)
                val byteArray: ByteArray = buf.array()
                render.native_eglSetImageData(bitmap.width, bitmap.height,byteArray)
            }
        } finally {
            try {
                `is`.close()
            } catch (e: IOException) {
                e.printStackTrace()
            }
        }
    }

    private fun createBitmapFromGLSurface(x: Int, y: Int, w: Int, h: Int): Bitmap? {
        val bitmapBuffer = IntArray(w * h)
        val bitmapSource = IntArray(w * h)
        val intBuffer: IntBuffer = IntBuffer.wrap(bitmapBuffer)
        intBuffer.position(0)
        try {
            GLES20.glReadPixels(
                x, y, w, h, GLES20.GL_RGBA, GLES20.GL_UNSIGNED_BYTE,
                intBuffer
            )
            var offset1: Int
            var offset2: Int
            for (i in 0 until h) {
                offset1 = i * w
                offset2 = (h - i - 1) * w
                for (j in 0 until w) {
                    val texturePixel = bitmapBuffer[offset1 + j]
                    val blue = texturePixel shr 16 and 0xff
                    val red = texturePixel shl 16 and 0x00ff0000
                    val pixel = texturePixel and -0xff0100 or red or blue
                    bitmapSource[offset2 + j] = pixel
                }
            }
        } catch (e: GLException) {
            return null
        }
        return Bitmap.createBitmap(bitmapSource, w, h, Bitmap.Config.ARGB_8888)
    }
}

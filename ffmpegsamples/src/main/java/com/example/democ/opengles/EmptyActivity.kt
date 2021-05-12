package com.example.democ.opengles

import android.app.ActivityManager
import android.content.Context
import android.content.pm.ConfigurationInfo
import android.graphics.BitmapFactory
import android.opengl.GLES20
import android.opengl.GLSurfaceView
import android.opengl.GLUtils
import android.os.Bundle
import androidx.fragment.app.FragmentActivity
import com.example.democ.R
import com.example.common_base.utils.array2Buffer
import com.example.democ.audio.log
import com.example.common_base.utils.createProgram
import com.example.common_base.utils.createTextureId
import kotlinx.android.synthetic.main.open_gles_layout.*
import javax.microedition.khronos.egl.EGLConfig
import javax.microedition.khronos.opengles.GL10


/**
 *
 * author : tanghongbin
 *
 * date   : 2020/6/2 16:00
 *
 * desc   : 这个类没什么用，留着做一些测试显示
 *
 **/
@Deprecated("")
class EmptyActivity : FragmentActivity() {

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        setContentView(R.layout.open_gles_layout)
        initEventAndData()

    }


    fun initEventAndData() {
        //检测系统是否支持OpenGL ES 2.0
        //检测系统是否支持OpenGL ES 2.0
        val activityManager: ActivityManager =
            getSystemService(Context.ACTIVITY_SERVICE) as ActivityManager
        val configurationInfo: ConfigurationInfo = activityManager.deviceConfigurationInfo
        val supportsEs2: Boolean = configurationInfo.reqGlEsVersion >= 0x20000
        log("当前是否支持:${supportsEs2}")

        if (supportsEs2) { // 请求一个OpenGL ES 2.0兼容的上下文
            mEmptySurface.setEGLContextClientVersion(2)
            // 设置我们的Demo渲染器，定义在后面讲
            mEmptySurface.setRenderer(LessonOneRenderer())
        } else { // 如果您想同时支持ES 1.0和2.0的话，这里您可以创建兼容OpenGL ES 1.0的渲染器
            return
        }
    }

    override fun onResume() {
        super.onResume()
        mEmptySurface.onResume()
    }


    override fun onPause() {
        super.onPause()
        mEmptySurface.onPause()
    }


    inner class LessonOneRenderer : GLSurfaceView.Renderer {

        private var vPositionLoc: Int = 0
        private var texCoordLoc: Int = 0
        private var textureLoc: Int = 0
        var mProgramHandle = 0
        var textureId = 0

        var vertexBuffer = array2Buffer(
            floatArrayOf(
                -1.0f, 1.0f, 0.0f,  // top left
                -1.0f, -1.0f, 0.0f,  // bottom left
                1.0f, -1.0f, 0.0f,  // bottom right
                1.0f, 1.0f, 0.0f  // top right
            )
        )
        var texBuffer = array2Buffer(
            floatArrayOf(
                0.0f, 1.0f,
                1.0f, 1.0f,
                1.0f, 0.0f,
                0.0f, 0.0f
            )
        )

        var index = shortArrayOf(0, 1, 2, 0, 2, 3)

        val indexBuffer = array2Buffer(index)

        override fun onDrawFrame(gl: GL10?) {
            GLES20.glUseProgram(mProgramHandle)
            //设置顶点数据
            vertexBuffer.position(0)
            GLES20.glEnableVertexAttribArray(vPositionLoc)
            GLES20.glVertexAttribPointer(vPositionLoc, 3, GLES20.GL_FLOAT, false, 0, vertexBuffer)
            //设置纹理顶点数据
            texBuffer.position(0)
            GLES20.glEnableVertexAttribArray(texCoordLoc)
            GLES20.glVertexAttribPointer(texCoordLoc, 2, GLES20.GL_FLOAT, false, 0, texBuffer)
            //设置纹理
            GLES20.glActiveTexture(GLES20.GL_TEXTURE0)
            GLES20.glBindTexture(GLES20.GL_TEXTURE_2D, textureId)
            GLES20.glUniform1i(textureLoc, 0)

            GLES20.glDrawElements(GLES20.GL_TRIANGLES, index.size, GLES20.GL_UNSIGNED_SHORT,indexBuffer)
        }

        override fun onSurfaceChanged(gl: GL10?, width: Int, height: Int) {

        }


        override fun onSurfaceCreated(gl: GL10?, config: EGLConfig?) { // 设置背景清理颜色为灰色
            mProgramHandle =
                createProgram(applicationContext)
//获取vPosition索引
            vPositionLoc = GLES20.glGetAttribLocation(mProgramHandle, "a_Position")
            texCoordLoc = GLES20.glGetAttribLocation(mProgramHandle, "a_TexCoordinate")
            textureLoc = GLES20.glGetUniformLocation(mProgramHandle, "u_Texture")
            val bitmap = BitmapFactory.decodeResource(resources, R.mipmap.live_buy)
            textureId = createTextureId()
            GLUtils.texImage2D(GLES20.GL_TEXTURE_2D, 0, bitmap, 0)

        }

    }

}
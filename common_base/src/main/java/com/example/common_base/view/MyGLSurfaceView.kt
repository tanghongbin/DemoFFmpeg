package com.example.common_base.view

import android.app.ActivityManager
import android.content.Context
import android.content.Context.ACTIVITY_SERVICE
import android.content.pm.ConfigurationInfo
import android.opengl.GLSurfaceView
import android.util.AttributeSet
import androidx.core.content.ContextCompat.getSystemService
import com.example.common_base.DemoApplication
import com.example.common_base.utils.log


class MyGLSurfaceView @JvmOverloads constructor(context: Context?, attrs: AttributeSet?) : GLSurfaceView(context, attrs){


    fun init(render:Renderer) {
        val activityManager: ActivityManager =
            DemoApplication.getInstance().getSystemService(Context.ACTIVITY_SERVICE) as ActivityManager
        val configurationInfo: ConfigurationInfo = activityManager.deviceConfigurationInfo
        val supportsEs3: Boolean = configurationInfo.reqGlEsVersion >= 0x30000

        if (supportsEs3){
            log("check is support v3 -${supportsEs3}")
            setEGLContextClientVersion(3)
        }

        /*If no setEGLConfigChooser method is called,
        then by default the view will choose an RGB_888 surface with a depth buffer depth of at least 16 bits.*/
//        setEGLConfigChooser(
//            8,
//            8,
//            8,
//            8,
//            16,
//            8
//        )
        setRenderer(render)
        renderMode = RENDERMODE_CONTINUOUSLY
       log("java layer config set success")
    }

}
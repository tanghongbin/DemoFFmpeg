package com.example.democ.opengles

import android.os.Build
import androidx.annotation.RequiresApi


@RequiresApi(Build.VERSION_CODES.KITKAT)
class NativeImageReaderRender {

    val GL_RENDER_TYPE = 0
    val CL_RENDER_TYPE = 1

    val IMAGE_FORMAT_RGBA = 0x01
    val IMAGE_FORMAT_NV21 = 0x02
    val IMAGE_FORMAT_NV12 = 0x03
    val IMAGE_FORMAT_I420 = 0x04

    val PARAM_TYPE_SET_SHADER_INDEX = 201

    companion object{
        init {
            System.loadLibrary("byteflow_render")
        }
    }

    private val mNativeContextHandle: Long = 0

    protected external fun native_CreateContext(renderType: Int)

    protected external fun native_DestroyContext()

    protected external fun native_Init(initType: Int): Int

    protected external fun native_UnInit(): Int

    protected external fun native_UpdateFrame(
        format: Int,
        data: ByteArray?,
        width: Int,
        height: Int
    )

    protected external fun native_LoadFilterData(
        index: Int,
        format: Int,
        width: Int,
        height: Int,
        bytes: ByteArray?
    )

    protected external fun native_LoadShaderScript(
        shaderIndex: Int,
        scriptStr: String?
    )

    protected external fun native_SetTransformMatrix(
        translateX: Float,
        translateY: Float,
        scaleX: Float,
        scaleY: Float,
        degree: Int,
        mirror: Int
    )

    protected external fun native_SetParamsInt(paramType: Int, value: Int)

    protected external fun native_GetParamsInt(paramType: Int): Int

    protected external fun native_OnSurfaceCreated()

    protected external fun native_OnSurfaceChanged(width: Int, height: Int)

    protected external fun native_OnDrawFrame()

}
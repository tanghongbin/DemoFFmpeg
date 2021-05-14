package com.example.common_base.utils

import android.content.Context
import android.opengl.GLES20
import android.view.View
import android.widget.ImageView
import com.bumptech.glide.Glide
import com.example.common_base.DemoApplication
import kotlinx.coroutines.*
import java.io.*
import java.nio.ByteBuffer
import java.nio.ByteOrder
import java.nio.FloatBuffer
import java.nio.ShortBuffer
import java.nio.charset.Charset

fun createProgram(context: Context): Int {
    val vertexCode =
        readAssetsTxt(
            context = context,
            filePath = "glsl/fbo/vetex.glsl"
        )
    val fragmentCode =
        readAssetsTxt(
            context = context,
            filePath = "glsl/fbo/fragment.glsl"
        )
    return createAndLinkProgram(
        vertexCode,
        fragmentCode
    )
}

fun createAndLinkProgram(vertexCode: String, fragmentCode: String): Int {
    //创建一个空的program
    var programHandle = GLES20.glCreateProgram()
    if (programHandle != 0) {
        //编译shader
        val vertexShaderHandle = compileShader(
            GLES20.GL_VERTEX_SHADER,
            vertexCode
        )
        val fragmentShaderHandle = compileShader(
            GLES20.GL_FRAGMENT_SHADER,
            fragmentCode
        )
        //绑定shader和program
        GLES20.glAttachShader(programHandle, vertexShaderHandle)
        GLES20.glAttachShader(programHandle, fragmentShaderHandle)
        //链接program
        GLES20.glLinkProgram(programHandle)

        val linkStatus = IntArray(1)
        //检测program状态
        GLES20.glGetProgramiv(programHandle, GLES20.GL_LINK_STATUS, linkStatus, 0)
        if (linkStatus[0] == 0) {
            log("Error link program:${GLES20.glGetProgramInfoLog(programHandle)}")
            //删除program
            GLES20.glDeleteProgram(programHandle)
            programHandle = 0
        }
    }
    if (programHandle == 0) {
        log("Error create program")
    }
    return programHandle
}

private fun compileShader(shaderType: Int, shaderSource: String): Int {
    //创建一个空shader
    var shaderHandle: Int = GLES20.glCreateShader(shaderType)
    if (shaderHandle != 0) {
        //加载shader源码
        GLES20.glShaderSource(shaderHandle, shaderSource)
        //编译shader
        GLES20.glCompileShader(shaderHandle)
        val compileStatus = IntArray(1)
        //检查shader状态
        GLES20.glGetShaderiv(shaderHandle, GLES20.GL_COMPILE_STATUS, compileStatus, 0)
        if (compileStatus[0] == 0) {
            //输入shader异常日志
            log("Error compile shader:${GLES20.glGetShaderInfoLog(shaderHandle)}")
            //删除shader
            GLES20.glDeleteShader(shaderHandle)
            shaderHandle = 0
        }
    }
    if (shaderHandle == 0) {
        log("Error create shader")
    }
    return shaderHandle
}

/**
 * 将一个输入流转化为字符串
 *
 * @param tInputStream
 * @return
 */
fun getStreamToStr(tInputStream: InputStream?): String? {
    if (tInputStream != null) {
        try {
            val tBufferedReader = BufferedReader(InputStreamReader(tInputStream))
            val tStringBuffer = StringBuffer()
            var sTempOneLine: String?
            while (tBufferedReader.readLine().also { sTempOneLine = it } != null) {
                tStringBuffer.append(sTempOneLine)
            }
            return tStringBuffer.toString()
        } catch (ex: Exception) {
            ex.printStackTrace()
        }
    }
    return null
}

fun readAssetsTxt(context: Context, filePath: String): String {
    try {
        val inputStream = context.assets.open(filePath)
        val size = inputStream.available()
        val buffer = ByteArray(size)
        inputStream.read(buffer)
        inputStream.close()
        return String(buffer, Charset.forName("utf-8"))
    } catch (e: IOException) {
        e.printStackTrace()
    }
    return ""
}

fun array2Buffer(array: FloatArray): FloatBuffer {
    val bb = ByteBuffer.allocateDirect(array.size * 4)
    bb.order(ByteOrder.nativeOrder())
    val buffer = bb.asFloatBuffer()
    buffer.put(array)
    buffer.position(0)
    return buffer
}

fun createTextureId(): Int {
    val textures = IntArray(1)
    GLES20.glGenTextures(1, textures, 0)
    GLES20.glBindTexture(GLES20.GL_TEXTURE_2D, textures[0])

    GLES20.glTexParameterf(
        GLES20.GL_TEXTURE_2D,
        GLES20.GL_TEXTURE_MIN_FILTER,
        GLES20.GL_LINEAR.toFloat()
    )
    GLES20.glTexParameterf(
        GLES20.GL_TEXTURE_2D,
        GLES20.GL_TEXTURE_MAG_FILTER,
        GLES20.GL_LINEAR.toFloat()
    )
    GLES20.glTexParameteri(
        GLES20.GL_TEXTURE_2D,
        GLES20.GL_TEXTURE_WRAP_S,
        GLES20.GL_CLAMP_TO_EDGE
    )
    GLES20.glTexParameteri(
        GLES20.GL_TEXTURE_2D,
        GLES20.GL_TEXTURE_WRAP_T,
        GLES20.GL_CLAMP_TO_EDGE
    )

    return textures[0]
}

fun array2Buffer(array: ShortArray): ShortBuffer {
    val bb = ByteBuffer.allocateDirect(array.size * 2)
    bb.order(ByteOrder.nativeOrder())
    val buffer = bb.asShortBuffer()
    buffer.put(array)
    buffer.position(0)
    return buffer
}


fun <T>Any.runAsyncTask(asyncBlock:() -> T,mainBlock:(T) -> Unit = {}){
    val job = Job()
    val scope = CoroutineScope(job)
    scope.launch(Dispatchers.IO) {
        val result = asyncBlock()
        withContext(Dispatchers.Main){
            mainBlock(result)
        }
        job.cancelAndJoin()
        log("协成任务结束")
    }
}

fun Any.inflateView(layoutId:Int):View{
    return View.inflate(DemoApplication.getInstance(),layoutId,null)
}

fun displayLocalImage(url:String?,imageView: ImageView){
    if (url.isNullOrBlank()){
        return
    }
    Glide.with(DemoApplication.getInstance())
        .load(url)
        .centerCrop()
        .into(imageView)
}
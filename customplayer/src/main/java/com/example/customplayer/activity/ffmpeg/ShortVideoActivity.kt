package com.example.customplayer.activity.ffmpeg

import android.annotation.SuppressLint
import android.os.Bundle
import androidx.appcompat.app.AppCompatActivity
import com.coder.ffmpeg.call.IFFmpegCallBack
import com.coder.ffmpeg.jni.FFmpegCommand
import com.coder.ffmpeg.utils.FFmpegUtils
import com.example.avutils.audio.recoder.AudioRecorder
import com.example.common_base.utils.FileUtils
import com.example.common_base.utils.log
import com.example.common_base.utils.runAsyncTask
import com.example.customplayer.R
import com.example.customplayer.interfaces.OnAvMergeListener
import com.example.customplayer.player.CustomMediaController
import com.example.customplayer.util.camera2.Camera2FrameCallback
import com.example.customplayer.util.camera2.Camera2Wrapper
import com.example.customplayer.util.getRamdowVideoPath
import com.example.customplayer.view.RecordButton
import kotlinx.android.synthetic.main.activity_gles_ffmpeg_muxer.*
import java.io.File

/**
 * 用opengles 渲染camera数据
 */
class ShortVideoActivity : AppCompatActivity(), Camera2FrameCallback {
    private val mMuxer by lazy { CustomMediaController(2, 4) }
    private val mCamera2Wrapper by lazy { Camera2Wrapper(this, this) }
    private val mAudioRecorder by lazy { AudioRecorder() }

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        setContentView(R.layout.activity_gles_ffmpeg_muxer)
        mAudioRecorder.setOnAudioFrameCapturedListener { audioData, ret ->
            mMuxer.native_audioData(audioData, ret)
        }
        mGLESMuxerSurface.init(mMuxer, true)
        mGLESMuxerSurface.holder
//        ImageReader
        mCamera2Wrapper.startCamera()
        mMuxer.setOnAvMergeListener(object : OnAvMergeListener {
            override fun onMerge(duration: Long, audioPath: String?, videoPath: String?) {
                val resultTime = duration/1000/1000
                log("打印时长:${resultTime}   音频地址：${audioPath}   视频地址：${videoPath}")
                FFmpegCommand.runCmd(getOutStr(resultTime, audioPath, videoPath),object : IFFmpegCallBack{
                    override fun onCancel() {

                    }

                    override fun onComplete() {
                        log("合并结束")
                        log("合并成功")
                    }

                    override fun onError(errorCode: Int, errorMsg: String?) {
                        log("合并失败 code:${errorCode}  msg:${errorMsg}")
                    }

                    override fun onProgress(progress: Int, pts: Long) {

                    }

                    override fun onStart() {

                    }

                })
            }
        })
        mRecordButton.setOnGestureListener(object : RecordButton.OnGestureListener {
            override fun onDown() {
                runAsyncTask({
                    mMuxer.initByType(2, 4)
                    mAudioRecorder.startCapture()
                    mMuxer.native_setSpeed(0.5)
                    mMuxer.native_startEncode()
                })
            }

            override fun onUp() {
                mMuxer.destroy()
            }

            override fun onClick() {

            }
        })

    }

    @SuppressLint("DefaultLocale")
    private fun getOutStr(duration: Long, audioPath: String?, videoPath: String?): Array<String?> {
        // ffmpeg
        // 其中，audioFile 为我们的 aac 文件的路径，videoFile 为 mp4 文件的路径，
        // output 为最终生成的 mp4 文件的路径，duration 为音频文件的长度，使用MediaExtractor 获取即可。
//        val commandStr:String = "-y -i $audioPath -ss 0 -t $duration -i $videoPath -acodec copy -vcodec copy ${}"
//        val arrays = commandStr.split(" ")
        val outputMixPath = getRamdowVideoPath("merged")
        if (!FileUtils.isFileExist(outputMixPath)){
            val file = File(outputMixPath)
            file.createNewFile()
        }

        val mixAudioCmd = FFmpegUtils.mixAudioVideo(videoPath, audioPath, duration.toInt(), outputMixPath)
        log("打印输出命令;${mixAudioCmd}")
        return mixAudioCmd
    }

    private fun listToArrays(list: List<String>):Array<String?>{
        return Array(list.size) { index ->
            list[index]
        }
    }

    override fun onDestroy() {
        mAudioRecorder.stopCapture()
        mCamera2Wrapper.stopCamera()
        super.onDestroy()
    }

    override fun onPause() {
        super.onPause()
    }

    override fun onCaptureFrame(data: ByteArray?, width: Int, height: Int) {
//        log("log onCaptureFrame data width:${width} height:${height} data:${data}")
    }

    override fun onPreviewFrame(data: ByteArray?, width: Int, height: Int) {
//        log("log onPreviewFrame data width:${width} height:${height} data:${data}")
        if (data == null) return
        mMuxer.native_onCameraFrameDataValible(2, data)
    }

}

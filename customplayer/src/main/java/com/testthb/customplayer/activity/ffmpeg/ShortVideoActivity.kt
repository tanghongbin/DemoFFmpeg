package com.testthb.customplayer.activity.ffmpeg

import android.annotation.SuppressLint
import android.content.Intent
import android.os.Bundle
import android.os.Handler
import android.os.Looper
import android.os.Message
import androidx.appcompat.app.AppCompatActivity
import com.flyco.tablayout.listener.CustomTabEntity
import com.flyco.tablayout.listener.OnTabSelectListener
import com.testthb.avutils.audio.recoder.AudioRecorder
import com.testthb.common_base.utils.*
import com.testthb.customplayer.R
import com.testthb.customplayer.bean.Speed
import com.testthb.customplayer.bean.TabEntity
import com.testthb.customplayer.interfaces.OnAvMergeListener
import com.testthb.customplayer.player.CustomMediaRecorder
import com.testthb.customplayer.player.MediaConstantsEnum
import com.testthb.customplayer.util.TimerUtils
import com.testthb.customplayer.util.camera2.Camera2FrameCallback
import com.testthb.customplayer.util.camera2.Camera2Wrapper
import com.testthb.customplayer.util.getRamdowVideoPath
import com.testthb.customplayer.util.runFFmpegCommand
import com.testthb.customplayer.view.RecordButton
import kotlinx.android.synthetic.main.activity_gles_ffmpeg_muxer.*
import java.io.BufferedOutputStream
import java.io.File
import java.io.FileOutputStream
import java.io.OutputStreamWriter
import java.util.concurrent.atomic.AtomicInteger
import kotlin.collections.set

/**
 * 用opengles 渲染camera数据
 */
class ShortVideoActivity : AppCompatActivity(), Camera2FrameCallback,
    TimerUtils.OnTimerUtilsListener {
    private val mMuxer by lazy { object : CustomMediaRecorder() {
        override fun getMuxerType(): MediaConstantsEnum {
            return MediaConstantsEnum.MUXER_SHORT_VIDEO
        }
    } }
    private val mCamera2Wrapper by lazy { Camera2Wrapper(this, this) }
    private val mAudioRecorder by lazy { AudioRecorder() }
    private val MAX_DURATION = 60 * 1000
    private val MIN_DURATION = 3
    //当前录制的进度
    private var mCurrentRecordProgress = 0
    //当前录制的时间
    private var mCurrentRecordDuration = 0
    //录制的计时器
    private var mTimer = TimerUtils(this, 20)
    private var mSpeed:Double = 0.0

    //录制速率
    private val mRecordSpeedTitles = arrayOf("极慢", "慢", "标准", "快", "极快")
    //用于装 TAB 对象的
    private val mCustomTabEntity = ArrayList<CustomTabEntity>()
    //TAB 选中的文字
    private val mSelectTextSize = 16f
    //TAB 未选中的文字
    private val mUnSelectTextSize = 12f
    private val mCount = AtomicInteger(0)
    private val TEMPPATH = "/storage/emulated/0/ffmpegtest/temp"

    /***
     * 转换完成的ts 目录
     */
    private val mWaitMergeTsList = ArrayList<String>()
    private val mMap = HashMap<String,Int>()
    private val MERGE_AV_TO_MP4 = 1 // 合成音 视频 为MP4
    private val CONVERT_MP4_TO_TS = 2
    private val PLAY_MP4 = 3
    private var mStartRecordTime = 0L

    private val mHandler = object : Handler(Looper.getMainLooper()) {
        override fun handleMessage(msg: Message) {
            when(msg.what){
                MERGE_AV_TO_MP4 -> {}
                CONVERT_MP4_TO_TS -> {
                    val outputMixPath = msg.obj as String
                    convertMp4ToTs(outputMixPath)
                }
                PLAY_MP4 -> {
                    log("打印合成之后的播放地址:${msg.obj}")
                    startActivity(Intent(this@ShortVideoActivity, PlayerDecodeFFmpegActivity::class.java).apply {
                        putExtra("url",msg.obj as String)
                    })
                }
            }
        }
    }

    @Suppress("RECEIVER_NULLABILITY_MISMATCH_BASED_ON_JAVA_ANNOTATIONS")
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
//        FFmpegCommand.setDebug(true)
        mMuxer.setOnAvMergeListener(object : OnAvMergeListener {
            override fun onMerge(duration: Long, audioPath: String?, videoPath: String?) {
                val resultTime = duration/1000/1000
                if (System.currentTimeMillis() - mStartRecordTime < 3 * 1000) {
                    toastSafe("录制时间过短")
                    FileUtils.deleteFile(audioPath)
                    FileUtils.deleteFile(videoPath)
                    return
                }
                    val outputMixPath = getRamdowVideoPath("merged")
                    runFFmpegCommand(getOutStr(resultTime, audioPath, videoPath,outputMixPath)) {
                        log("合并成功   complete 线程：${Thread.currentThread().name}")
                        FileUtils.deleteFile(audioPath)
                        FileUtils.deleteFile(videoPath)
                        mMap[outputMixPath] = resultTime.toInt()
                        mHandler.sendMessageDelayed(Message.obtain(mHandler,CONVERT_MP4_TO_TS,outputMixPath),300)
                    }
            }
        })
        mRecordButton.setOnGestureListener(object : RecordButton.OnGestureListener {
            override fun onDown() {
                goneViews(douyin_button_speed_tab,mContactVideo)
                mTimer.start((MAX_DURATION * mSpeed).toInt())
                mAudioRecorder.startCapture()
                mMuxer.native_initEncode()
                mStartRecordTime = System.currentTimeMillis()
            }

            override fun onUp() {
                showViews(douyin_button_speed_tab,mContactVideo)
                mTimer.stop()
                mProgressView.addProgress(mCurrentRecordProgress * 1.0F / mCurrentRecordDuration)
                mMuxer.native_stopEncode()
            }

            override fun onClick() {

            }
        })
        mContactVideo.setOnClickListener {
            if (mWaitMergeTsList.isEmpty()) {
                return@setOnClickListener
            }
            val outputMixPath = getRamdowVideoPath("contact")
            if (!FileUtils.isFileExist(outputMixPath)){
                File(outputMixPath).createNewFile()
            }
            val tsFilesPath = "${TEMPPATH}/tsFile.txt"
            if (FileUtils.isFileExist(tsFilesPath)){
                FileUtils.deleteFile(tsFilesPath)
            }
            val tsFiles = File(tsFilesPath)
            tsFiles.createNewFile()
            val tsStreams = OutputStreamWriter(BufferedOutputStream(FileOutputStream(tsFiles)))
            mWaitMergeTsList.forEachIndexed { index,mergeTsDir ->
                if (FileUtils.isFolderExist(mergeTsDir)) {
                    val folder = File(mergeTsDir)
                    folder.listFiles { dir, name -> name.endsWith(".ts", true) }.forEach { file ->
                        tsStreams.write("file ${file.path}\n")
                    }
                }
            }
            tsStreams.flush()
            tsStreams.close()
                val command =
//                    "ffmpeg -f mpegts -i \"concat:${resultPaths}\" -c copy -bsf:a aac_adtstoasc $outputMixPath"
                    "ffmpeg -y -f concat -safe 0 -i $tsFilesPath -c copy $outputMixPath"
                log("打印合并命令行:${command}")
                runFFmpegCommand(command) {
                    log("转成合并完成 ")
                    mWaitMergeTsList.forEach {
                        FileUtils.deleteFile(it)
                    }
                    mWaitMergeTsList.clear()
                    FileUtils.deleteFile(TEMPPATH)
                    mProgressView.clear()
                    val msg =  Message.obtain(mHandler,PLAY_MP4,outputMixPath)
                    mHandler.sendMessageDelayed(msg,1500)
                    finish()
                }
        }
        muxerButton.setOnClickListener {
            val tsPath = "/storage/emulated/0/ffmpegtest/encodeVideos/tests.mp4"
            convertMp4ToTs(tsPath)
        }
        initRecordSpeedTab()

    }





    private fun convertMp4ToTs(mergePath: String) {
        val tsDir = "/storage/emulated/0/ffmpegtest/temp/tsdir${System.currentTimeMillis()}"
        val tsPath = tsDir +  "/output${mCount.incrementAndGet()}.m3u8"
        if (!FileUtils.isFolderExist(tsDir)) {
            File(tsDir).mkdirs()
        }
        if (!FileUtils.isFileExist(tsPath)) {
            File(tsPath).createNewFile()
        }

//        val command = "ffmpeg -y -i $mergePath -vcodec copy -acodec copy -vbsf h264_mp4toannexb $tsPath"
//        val command = "ffmpeg -i $mergePath $tsPath"
        //                val command = "ffmpeg -y -f concat -i $filesPath -codec copy $outputMixPath"
            log("mp4 2 ts 线程:${Thread.currentThread().name}")
//            val command = "ffmpeg -i $mergePath -c copy -bsf:v h264_mp4toannexb -f mpegts $tsPath"
            var command = "ffmpeg -y -i %s -c copy -bsf:v h264_mp4toannexb -hls_time %s %s"
            command = String.format(command, mergePath, mMap[mergePath] ?: 0, tsPath)
            log("打印命令行:${command}")
            runFFmpegCommand(command) {
                log("转成ts完成 ：${tsPath}")
                FileUtils.deleteFile(mergePath)
                mWaitMergeTsList.add(tsDir)
            }
    }

    @SuppressLint("DefaultLocale")
    private fun getOutStr(duration: Long, audioPath: String?, videoPath: String?,outputMixPath:String?): String {
        // ffmpeg
        // 其中，audioFile 为我们的 aac 文件的路径，videoFile 为 mp4 文件的路径，
        // output 为最终生成的 mp4 文件的路径，duration 为音频文件的长度，使用MediaExtractor 获取即可。
//        val arrays = commandStr.split(" ")
        if (!FileUtils.isFileExist(outputMixPath)){
            val file = File(outputMixPath)
            file.createNewFile()
        }
        val commandStr:String = "ffmpeg -y -i $audioPath -ss 0 -t $duration -i $videoPath -acodec copy -vcodec copy ${outputMixPath}"

//        val mixAudioCmd = FFmpegUtils.mixAudioVideo(videoPath, audioPath, duration.toInt(), outputMixPath)
        log("打印输出命令;${commandStr}")
        return commandStr
    }

    private fun listToArrays(list: List<String>):Array<String?>{
        return Array(list.size) { index ->
            list[index]
        }
    }

    override fun onDestroy() {
        mAudioRecorder.stopCapture()
        mCamera2Wrapper.stopCamera()
        mMuxer.destroy()
        super.onDestroy()
    }

    override fun onPause() {
        super.onPause()
    }

    override fun onBackPressed() {
        if (mWaitMergeTsList.isNotEmpty()){
            mWaitMergeTsList.forEach { path ->
                FileUtils.deleteFile(path)
            }
            mWaitMergeTsList.clear()
            mProgressView.clear()
        } else {
            super.onBackPressed()
        }
    }

    override fun onCaptureFrame(data: ByteArray?, width: Int, height: Int) {
//        log("log onCaptureFrame data width:${width} height:${height} data:${data}")
    }

    override fun onPreviewFrame(data: ByteArray?, width: Int, height: Int) {
//        log("log onPreviewFrame data width:${width} height:${height} data:${data}")
        if (data == null) return
        mMuxer.native_onCameraFrameDataValible(2, data)
    }

    override fun update(timer: TimerUtils, currentTime: Int) {
        mCurrentRecordProgress = currentTime
        mCurrentRecordDuration = timer.duration
        mProgressView.setLoadingProgress(currentTime * 1.0f / timer.duration)
    }

    override fun end(timer: TimerUtils) {

    }

    /**
     * 初始化录制速率 TAB
     */
    private fun initRecordSpeedTab() {
        mCustomTabEntity.clear()
        for (title in mRecordSpeedTitles)
            mCustomTabEntity.add(TabEntity(title))
        douyin_button_speed_tab.setTabData(mCustomTabEntity)
        douyin_button_speed_tab.setTextsize(mSelectTextSize, mUnSelectTextSize)
        douyin_button_speed_tab.currentTab = 2
        selectTabPos(2)
        douyin_button_speed_tab.setOnTabSelectListener(object : OnTabSelectListener {
            override fun onTabSelect(position: Int) {
                selectTabPos(position)
            }

            override fun onTabReselect(position: Int) {
            }
        })

    }

    private fun selectTabPos(position: Int) {
        when (douyin_button_speed_tab.getTitleView(position).text) {
            mRecordSpeedTitles[0] -> {
                mSpeed = Speed.VERY_SLOW.value
            }
            mRecordSpeedTitles[1] -> {
                mSpeed = Speed.SLOW.value
            }
            mRecordSpeedTitles[2] -> {
                mSpeed = Speed.NORMAL.value
            }
            mRecordSpeedTitles[3] -> {
                mSpeed = Speed.FAST.value
            }
            mRecordSpeedTitles[4] -> {
                mSpeed = Speed.VERY_FAST.value
            }
            else -> {
                mSpeed = Speed.NORMAL.value
            }
        }
        mMuxer.native_setSpeed(mSpeed)
        douyin_button_speed_tab.setTextsize(mSelectTextSize, mUnSelectTextSize)
    }
}

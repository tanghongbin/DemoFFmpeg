package com.testthb.customplayer.activity

import android.content.Intent
import android.os.*
import androidx.appcompat.app.AppCompatActivity
import androidx.recyclerview.widget.GridLayoutManager
import com.testthb.common_base.utils.log
import com.testthb.common_base.utils.requestCustomPermissions
import com.testthb.customplayer.R
import com.testthb.customplayer.activity.cameralearn.CameraLearnActivity
import com.testthb.customplayer.activity.ffmpeg.*
import com.testthb.customplayer.activity.hw.PlayerDecodeHwActivity
import com.testthb.customplayer.activity.hw.PlayerDecodeHwOpenGLESActivity
import com.testthb.customplayer.adapter.PlayerListAdapter
import com.testthb.customplayer.module.PlayerModule
import com.testthb.customplayer.opus.OpusOperationActivity
import io.reactivex.Observable
import io.reactivex.android.schedulers.AndroidSchedulers
import io.reactivex.rxjava3.schedulers.Schedulers
import kotlinx.android.synthetic.main.activity_custom_player_list.*

class CustomPlayerListActivity : AppCompatActivity() {

    private val mModule = PlayerModule()

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        setContentView(R.layout.activity_custom_player_list)
        requestCustomPermissions {
            init()
//            startActivity()
        }
//        TestString.test()
        val dis = Observable.just("")
            .subscribeOn(io.reactivex.schedulers.Schedulers.io())
            .map {
                log("stupid map:${Thread.currentThread().name}")
            }
            .observeOn(AndroidSchedulers.mainThread())
            .subscribe {
                log("subscribe :${Thread.currentThread().name}")
            }
//        dis.dispose()
        Handler().postDelayed(Runnable {  },500)
    }


    private fun init() {
        val layoutManager = GridLayoutManager(this, 2)
        mRecyclePlayer.layoutManager = layoutManager
        val adapter = PlayerListAdapter()
        adapter.setOnItemClickListener { i, videoInfo ->
//            val classes = PlayerDecodeHwActivity::class.java
            val classes = AvEditorActivity::class.java
            startActivity(Intent(this, classes).apply {
                putExtra("url",videoInfo.data)
            })
        }
        adapter.setOnItemLongClickListener { i, videoInfo ->

        }
        mRecyclePlayer.adapter = adapter
        mModule.loadVideoList {
            adapter.addList(it)
        }
        startMuxer.setOnClickListener {
            startActivity(Intent(this,FFmpegGLESMuxerActivity::class.java))
        }
        startWebrtc.setOnClickListener {
            startActivity(Intent(this,WebrtcActivity::class.java))
        }
        startLive.setOnClickListener {
            startActivity(Intent(this,RtmpGLESActivity::class.java))
        }
        recordShortVideo.setOnClickListener {
//            Test().test<String>()
            startActivity(Intent(this,ShortVideoActivity::class.java))
        }
        camera2Learn.setOnClickListener {
            startActivity(Intent(this,CameraLearnActivity::class.java))
        }
        opus.setOnClickListener {
            startActivity(Intent(this,OpusOperationActivity::class.java))
        }

    }
}
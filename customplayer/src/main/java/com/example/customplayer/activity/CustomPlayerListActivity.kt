package com.example.customplayer.activity

import android.content.Intent
import androidx.appcompat.app.AppCompatActivity
import android.os.Bundle
import androidx.recyclerview.widget.GridLayoutManager
import com.example.common_base.utils.log
import com.example.common_base.utils.requestCustomPermissions
import com.example.customplayer.R
import com.example.customplayer.adapter.PlayerListAdapter
import com.example.customplayer.module.PlayerModule
import com.example.customplayer.player.CustomPlayer
import kotlinx.android.synthetic.main.activity_custom_player_list.*

class CustomPlayerListActivity : AppCompatActivity() {

    private val mModule = PlayerModule()

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        setContentView(R.layout.activity_custom_player_list)
        requestCustomPermissions {
            init()
        }
    }

    private fun init() {
        val layoutManager = GridLayoutManager(this, 2)
        mRecyclePlayer.layoutManager = layoutManager
        val adapter = PlayerListAdapter()
        adapter.setOnItemClickListener { i, videoInfo ->
            startActivity(Intent(this,PlayerDecodeHwActivity::class.java).apply {
                putExtra("url",videoInfo.data)
            })
        }
        mRecyclePlayer.adapter = adapter
        mModule.loadVideoList {
            adapter.addList(it)
        }
        val player = CustomPlayer()
        log(player.nativeGetInfo())
    }
}
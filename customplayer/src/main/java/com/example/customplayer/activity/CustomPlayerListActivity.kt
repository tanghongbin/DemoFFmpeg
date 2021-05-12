package com.example.customplayer.activity

import androidx.appcompat.app.AppCompatActivity
import android.os.Bundle
import androidx.recyclerview.widget.DividerItemDecoration
import androidx.recyclerview.widget.GridLayoutManager
import com.example.customplayer.R
import com.example.customplayer.adapter.PlayerListAdapter
import com.example.customplayer.module.PlayerModule
import kotlinx.android.synthetic.main.activity_custom_player_list.*

class CustomPlayerListActivity : AppCompatActivity() {

    private val mModule = PlayerModule()

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        setContentView(R.layout.activity_custom_player_list)
        val layoutManager = GridLayoutManager(this,2)
        mRecyclePlayer.layoutManager = layoutManager
        val adapter = PlayerListAdapter()
        mRecyclePlayer.adapter = adapter
        mModule.loadVideoList {
            adapter.addList(it)
        }
    }
}
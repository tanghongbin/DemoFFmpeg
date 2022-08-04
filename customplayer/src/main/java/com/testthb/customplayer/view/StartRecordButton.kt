package com.testthb.customplayer.view

import android.content.Context
import android.util.AttributeSet
import android.view.View
import androidx.appcompat.widget.AppCompatImageView
import com.testthb.customplayer.R

class StartRecordButton @JvmOverloads constructor(
    context: Context, attrs: AttributeSet? = null
) : AppCompatImageView(context, attrs), View.OnClickListener {
    private var isStart = false
    private var startBlock = {}
    private var stopBlock = {}
    init {
        setImageResource(R.mipmap.live_resume)
        setOnClickListener(this)
    }

    override fun onClick(v: View?) {
        isStart = !isStart
        if (isStart) startBlock.invoke() else stopBlock.invoke()
        setImageResource(if (isStart) R.mipmap.live_pause else R.mipmap.live_resume)
    }

    fun setStartListener(start:() -> Unit) {
        startBlock = start
    }

    fun setStopListener(stop:() -> Unit) {
        stopBlock = stop
    }
}
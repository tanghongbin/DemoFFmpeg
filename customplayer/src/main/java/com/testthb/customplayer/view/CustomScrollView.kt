package com.testthb.customplayer.view

import android.content.Context
import android.util.AttributeSet
import android.view.MotionEvent
import android.widget.FrameLayout
import com.testthb.common_base.utils.log

class CustomScrollView @JvmOverloads constructor(
    context: Context, attrs: AttributeSet? = null
) : FrameLayout(context, attrs) {

    override fun onTouchEvent(event: MotionEvent?): Boolean {
        log("log onInterceptTouchEvent:${event?.action}")
        return super.onTouchEvent(event)
    }

    override fun onInterceptTouchEvent(ev: MotionEvent?): Boolean {
        log("log onInterceptTouchEvent:${ev?.action}")
        return super.onInterceptTouchEvent(ev)
    }

    private var moveDistance = 0f

    override fun dispatchTouchEvent(ev: MotionEvent?): Boolean {
        log("log dispatchevent:${ev?.action}")
        when(ev?.action) {
            MotionEvent.ACTION_DOWN -> {
                moveDistance = ev?.x
            }
            MotionEvent.ACTION_MOVE -> {
                scrollBy( (ev!!.x - moveDistance).toInt(),0)
            }
        }
        return super.dispatchTouchEvent(ev)
    }
}
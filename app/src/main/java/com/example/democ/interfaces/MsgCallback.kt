package com.example.democ.interfaces

interface MsgCallback {
    fun callback(type: Int)

    fun renderSizeDimensionChanged(renderWidth:Int,rederHeight:Int){

    }

    fun processRangeSetup(min:Int,max:Int){

    }

    fun processChanged(current:Int){

    }
}
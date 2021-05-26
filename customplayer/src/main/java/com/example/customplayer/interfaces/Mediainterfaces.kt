package com.example.customplayer.interfaces

interface OnPreparedListener{
    fun onPrepared()
}
interface OnVideoSizeChangeListener{
    fun onVideoSize(width:Int,height:Int)
}
interface OnSeekProgressChangeListener{
    fun onProgress(progress:Int)
}
interface OnDurationListener{
    fun onDuration(min:Int,max:Int)
}
interface OnCompleteListener{
    fun onComplete()
}
interface OnErrorListener{
    fun onError(code:Int,str:String)
}


package com.testthb.customplayer.util

object CustomFFmpegCmdUtils {
    fun cutVideo(srcUrl:String,startTime:Long,duration:Long,outputPath:String):String{
        val command = "ffmpeg -ss $startTime -t $duration -y -accurate_seek -i $srcUrl -codec copy -avoid_negative_ts 1 $outputPath"
        return command
    }
}
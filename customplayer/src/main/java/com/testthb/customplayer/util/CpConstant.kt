package com.testthb.customplayer.util

import com.testthb.common_base.utils.FileUtils


const val AV_PREFIX = "/storage/emulated/0/ffmpegtest/encodeVideos/"


fun getRamdowVideoPath(suffix:String = "randow"):String {
     if (!FileUtils.isFolderExist(AV_PREFIX)){
         FileUtils.makeFolders(AV_PREFIX);
     }
     return "${AV_PREFIX}${System.currentTimeMillis()}-${suffix}.mp4"
}


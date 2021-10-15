package com.example.customplayer.util

import com.example.common_base.utils.FileUtils


const val AV_PREFIX = "/storage/emulated/0/ffmpegtest/encodeVideos/"


 fun getRamdowVideoPath():String {
     if (!FileUtils.isFolderExist(AV_PREFIX)){
         FileUtils.makeFolders(AV_PREFIX);
     }
     return "${AV_PREFIX}${System.currentTimeMillis()}-randow.mp4"
}
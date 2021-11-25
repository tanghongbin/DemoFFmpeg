package com.testthb.customplayer.module

import android.database.Cursor
import android.graphics.Bitmap
import android.media.MediaMetadataRetriever
import android.provider.MediaStore
import com.testthb.common_base.DemoApplication
import com.testthb.common_base.utils.FileUtils
import com.testthb.common_base.utils.MD5Utils
import com.testthb.common_base.utils.runAsyncTask
import com.testthb.customplayer.bean.VideoInfo
import java.io.File
import java.io.FileOutputStream
import java.util.*
import kotlin.collections.ArrayList


class PlayerModule {

    private val tempPathDir by lazy { DemoApplication.getInstance().cacheDir.path + File.separator + "demoFFmpegThubm" }
    private val mVideoList = mutableListOf<VideoInfo>()

    private val sLocalVideoColumns = arrayOf<String>(
        MediaStore.Video.Media._ID,  // 视频id
        MediaStore.Video.Media.DATA,  // 视频路径
        MediaStore.Video.Media.SIZE,  // 视频字节大小
        MediaStore.Video.Media.DISPLAY_NAME,  // 视频名称 xxx.mp4
        MediaStore.Video.Media.TITLE,  // 视频标题
        MediaStore.Video.Media.DATE_ADDED,  // 视频添加到MediaProvider的时间
        MediaStore.Video.Media.DATE_MODIFIED,  // 上次修改时间，该列用于内部MediaScanner扫描，外部不要修改
        MediaStore.Video.Media.MIME_TYPE,  // 视频类型 video/mp4
        MediaStore.Video.Media.DURATION,  // 视频时长
        MediaStore.Video.Media.ARTIST,  // 艺人名称
        MediaStore.Video.Media.ALBUM,  // 艺人专辑名称
        MediaStore.Video.Media.RESOLUTION,  // 视频分辨率 X x Y格式
        MediaStore.Video.Media.DESCRIPTION,  // 视频描述
        MediaStore.Video.Media.IS_PRIVATE,
        MediaStore.Video.Media.TAGS,
        MediaStore.Video.Media.CATEGORY,  // YouTube类别
        MediaStore.Video.Media.LANGUAGE,  // 视频使用语言
        MediaStore.Video.Media.LATITUDE,  // 拍下该视频时的纬度
        MediaStore.Video.Media.LONGITUDE,  // 拍下该视频时的经度
        MediaStore.Video.Media.DATE_TAKEN,
        MediaStore.Video.Media.MINI_THUMB_MAGIC,
        MediaStore.Video.Media.BUCKET_ID,
        MediaStore.Video.Media.BUCKET_DISPLAY_NAME,
        MediaStore.Video.Media.BOOKMARK // 上次视频播放的位置
    )
    private val sLocalVideoThumbnailColumns = arrayOf<String>(
        MediaStore.Video.Thumbnails.DATA,  // 视频缩略图路径
        MediaStore.Video.Thumbnails.VIDEO_ID,  // 视频id
        MediaStore.Video.Thumbnails.KIND,
        MediaStore.Video.Thumbnails.WIDTH,  // 视频缩略图宽度
        MediaStore.Video.Thumbnails.HEIGHT // 视频缩略图高度
    )

    fun loadVideoList(block:(List<VideoInfo>) -> Unit){
        DemoApplication.getInstance().runAsyncTask({
            internalLoadVideoList()
        },{
            mVideoList.addAll(it)
            block(it)
        })
    }

    @Suppress("NULLABILITY_MISMATCH_BASED_ON_JAVA_ANNOTATIONS")
    private fun internalLoadVideoList():List<VideoInfo>{
        val videoList = ArrayList<VideoInfo>()
        val cursor: Cursor? = DemoApplication.getInstance().contentResolver.query(
            MediaStore.Video.Media.EXTERNAL_CONTENT_URI, sLocalVideoColumns,
            null, null, null)
        // 每次要删除所有缩率图文件
        if (cursor != null && cursor.moveToFirst()) {
            do {
                val videoInfo = VideoInfo()
                val id = cursor.getInt(cursor.getColumnIndex(MediaStore.Video.Media._ID))
                val data = cursor.getString(cursor.getColumnIndex(MediaStore.Video.Media.DATA))
                val size = cursor.getLong(cursor.getColumnIndex(MediaStore.Video.Media.SIZE))
                val displayName = cursor.getString(cursor.getColumnIndex(MediaStore.Video.Media.DISPLAY_NAME))
                val title = cursor.getString(cursor.getColumnIndex(MediaStore.Video.Media.TITLE))
                val dateAdded = cursor.getLong(cursor.getColumnIndex(MediaStore.Video.Media.DATE_ADDED))
                val dateModified = cursor.getLong(cursor.getColumnIndex(MediaStore.Video.Media.DATE_MODIFIED))
                val mimeType = cursor.getString(cursor.getColumnIndex(MediaStore.Video.Media.MIME_TYPE))
                val duration = cursor.getLong(cursor.getColumnIndex(MediaStore.Video.Media.DURATION))
                val artist = cursor.getString(cursor.getColumnIndex(MediaStore.Video.Media.ARTIST))
                val album = cursor.getString(cursor.getColumnIndex(MediaStore.Video.Media.ALBUM))
                val resolution = cursor.getString(cursor.getColumnIndex(MediaStore.Video.Media.RESOLUTION))
                val description = cursor.getString(cursor.getColumnIndex(MediaStore.Video.Media.DESCRIPTION))
                val isPrivate = cursor.getInt(cursor.getColumnIndex(MediaStore.Video.Media.IS_PRIVATE))
                val tags = cursor.getString(cursor.getColumnIndex(MediaStore.Video.Media.TAGS))
                val category = cursor.getString(cursor.getColumnIndex(MediaStore.Video.Media.CATEGORY))
                val latitude = cursor.getDouble(cursor.getColumnIndex(MediaStore.Video.Media.LATITUDE))
                val longitude = cursor.getDouble(cursor.getColumnIndex(MediaStore.Video.Media.LONGITUDE))
                val dateTaken = cursor.getInt(cursor.getColumnIndex(MediaStore.Video.Media.DATE_TAKEN))
                val miniThumbMagic = cursor.getInt(cursor.getColumnIndex(MediaStore.Video.Media.MINI_THUMB_MAGIC))
                val bucketId = cursor.getString(cursor.getColumnIndex(MediaStore.Video.Media.BUCKET_ID))
                val bucketDisplayName = cursor.getString(cursor.getColumnIndex(MediaStore.Video.Media.BUCKET_DISPLAY_NAME))
                val bookmark = cursor.getInt(cursor.getColumnIndex(MediaStore.Video.Media.BOOKMARK))
                // 计算duration
                val calendar = Calendar.getInstance()
                calendar.timeZone = TimeZone.getTimeZone("Asia/Shanghai")
                calendar.timeInMillis = duration
                val hourInt = calendar.get(Calendar.HOUR) - 8
                val hour = if (hourInt < 10) "0${hourInt}" else hourInt.toString()
                val minute = if (calendar.get(Calendar.MINUTE) < 10) "0${calendar.get(Calendar.MINUTE)}" else calendar.get(Calendar.MINUTE).toString()
                val second = if (calendar.get(Calendar.SECOND) < 10) "0${calendar.get(Calendar.SECOND)}" else calendar.get(Calendar.SECOND).toString()
                val timeResult = "${hour}:${minute}:${second}"
                videoInfo.durationTime = timeResult
//                val thumbnailCursor: Cursor = DemoApplication.instance.contentResolver.query(
//                    MediaStore.Video.Thumbnails.EXTERNAL_CONTENT_URI,
//                    sLocalVideoThumbnailColumns,
//                    MediaStore.Video.Thumbnails.VIDEO_ID + "=" + id,
//                    null,
//                    null
//                )
//                if (thumbnailCursor != null && thumbnailCursor.moveToFirst()) {
//                    do {
//                        val thumbnailData =
//                            thumbnailCursor.getString(thumbnailCursor.getColumnIndex(MediaStore.Video.Thumbnails.DATA))
//                        val kind =
//                            thumbnailCursor.getInt(thumbnailCursor.getColumnIndex(MediaStore.Video.Thumbnails.KIND))
//                        val width =
//                            thumbnailCursor.getLong(thumbnailCursor.getColumnIndex(MediaStore.Video.Thumbnails.WIDTH))
//                        val height =
//                            thumbnailCursor.getLong(thumbnailCursor.getColumnIndex(MediaStore.Video.Thumbnails.HEIGHT))
//                        videoInfo.thumbnailData = thumbnailData
//                        videoInfo.kind = kind
//                        videoInfo.width = width
//                        videoInfo.height = height
//                    } while (thumbnailCursor.moveToNext())
//                    thumbnailCursor.close()
//                }
                videoInfo.thumbnailData = getVideoThumbnail(data)
                videoInfo.id = id
                videoInfo.data = data
                videoInfo.size = size
                videoInfo.displayName = displayName
                videoInfo.title = title
                videoInfo.dateAdded = dateAdded
                videoInfo.dateModified = dateModified
                videoInfo.mimeType = mimeType
                videoInfo.duration = duration
                videoInfo.artist = artist
                videoInfo.album = album
                videoInfo.resolution = resolution
                videoInfo.description = description
                videoInfo.isPrivate = isPrivate
                videoInfo.tags = tags
                videoInfo.category = category
                videoInfo.latitude = latitude
                videoInfo.longitude = longitude
                videoInfo.dateTaken = dateTaken
                videoInfo.miniThumbMagic = miniThumbMagic
                videoInfo.bucketId = bucketId
                videoInfo.bucketDisplayName = bucketDisplayName
                videoInfo.bookmark = bookmark
//                log("videoInfo = $videoInfo")
                videoList.add(videoInfo)
            } while (cursor.moveToNext())
            cursor.close()
        }
        return videoList
    }


    // 获取视频缩略图
    fun getVideoThumbnail(videoUrl: String?): String? {
        if (!FileUtils.isFolderExist(tempPathDir)){
            val fileFolder = File(tempPathDir)
            fileFolder.mkdirs()
        }
        val md5 = MD5Utils.getMD5String(videoUrl)
        val fileName = tempPathDir + File.separator + "${md5}.jpg"
        val file = File(fileName)
        if (file.exists()){
            return fileName
        }
        var b: Bitmap? = null
        val retriever = MediaMetadataRetriever()
        try {
            retriever.setDataSource(videoUrl)
            b = retriever.frameAtTime
            file.createNewFile()
            val outStream = FileOutputStream(file)
            b?.compress(Bitmap.CompressFormat.JPEG,100,outStream)
            outStream.flush()
            outStream.close()
            b?.recycle()
        } catch (e: IllegalArgumentException) {
//            e.printStackTrace()
        } catch (e: RuntimeException) {
//            e.printStackTrace()
        }catch (e:Exception){
//            e.printStackTrace()
        } finally {
            try {
                retriever.release()
            } catch (e: RuntimeException) {
//                e.printStackTrace()
            }
        }
        return fileName
    }

    fun destroy(){
    }
}
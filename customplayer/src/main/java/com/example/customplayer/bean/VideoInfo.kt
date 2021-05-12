package com.example.customplayer.bean

class VideoInfo {
     var id = 0
     var data: String? = null
     var size: Long = 0
     var displayName: String? = null
     var title: String? = null
     var dateAdded: Long = 0
     var dateModified: Long = 0
     var mimeType: String? = null
     var duration: Long = 0
     var artist: String? = null
     var album: String? = null
     var resolution: String? = null
     var description: String? = null
     var isPrivate = 0
     var tags: String? = null
     var category: String? = null
     var latitude = 0.0
     var longitude = 0.0
     var dateTaken = 0
     var miniThumbMagic = 0
     var bucketId: String? = null
     var bucketDisplayName: String? = null
     var bookmark = 0
     var thumbnailData: String? = null
     var kind = 0
     var width: Long = 0
     var height: Long = 0
     var durationTime:String? = null
    override fun toString(): String {
        return "VideoInfo{" +
                "id=" + id +
                ", data='" + data + '\'' +
                ", size=" + size +
                ", displayName='" + displayName + '\'' +
                ", title='" + title + '\'' +
                ", dateAdded=" + dateAdded +
                ", dateModified=" + dateModified +
                ", mimeType='" + mimeType + '\'' +
                ", duration=" + duration +
                ", artist='" + artist + '\'' +
                ", album='" + album + '\'' +
                ", resolution='" + resolution + '\'' +
                ", description='" + description + '\'' +
                ", isPrivate=" + isPrivate +
                ", tags='" + tags + '\'' +
                ", category='" + category + '\'' +
                ", latitude=" + latitude +
                ", longitude=" + longitude +
                ", dateTaken=" + dateTaken +
                ", miniThumbMagic=" + miniThumbMagic +
                ", bucketId='" + bucketId + '\'' +
                ", bucketDisplayName='" + bucketDisplayName + '\'' +
                ", bookmark=" + bookmark +
                ", thumbnailData='" + thumbnailData + '\'' +
                ", kind=" + kind +
                ", width=" + width +
                ", height=" + height +
                '}'
    }
}
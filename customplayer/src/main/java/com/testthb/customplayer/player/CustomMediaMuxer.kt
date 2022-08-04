package com.testthb.customplayer.player

open class CustomMediaMuxer : CustomMediaController() {
    override fun getRootType(): MediaConstantsEnum {
        return MediaConstantsEnum.MEDIA_MUXER
    }

    override fun getMuxerType(): MediaConstantsEnum {
        return MediaConstantsEnum.MUXER_RECORD_HW
    }

    override fun getPlayerType(): MediaConstantsEnum {
        return MediaConstantsEnum.NONE
    }
}
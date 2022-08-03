package com.testthb.customplayer.player

class CustomMediaPlayer : CustomMediaController() {
    override fun getRootType(): MediaConstantsEnum {
        return MediaConstantsEnum.MEDIA_PLAYER
    }

    override fun getMuxerType(): MediaConstantsEnum {
        return MediaConstantsEnum.NONE
    }

    override fun getPlayerType(): MediaConstantsEnum {
        return MediaConstantsEnum.MUXER_RECORD_HW
    }

}
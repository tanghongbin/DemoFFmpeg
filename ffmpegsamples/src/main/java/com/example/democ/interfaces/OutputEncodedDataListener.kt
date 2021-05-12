package com.example.democ.interfaces

import android.media.MediaCodec
import android.media.MediaFormat
import java.nio.ByteBuffer

public interface OutputEncodedDataListener{
        
        fun outputData( byteBuffer: ByteBuffer, bufferInfo: MediaCodec.BufferInfo)
        
        fun outputFormatChanged(mediaFormat: MediaFormat)
        
    }
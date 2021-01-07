/*
 *  COPYRIGHT NOTICE
 *  Copyright (C) 2016, Jhuster <lujun.hust@gmail.com>
 *  https://github.com/Jhuster/AudioDemo
 *
 *  @license under the Apache License, Version 2.0
 *
 *  @file    AudioEncoder.java
 *
 *  @version 1.0
 *  @author  Jhuster
 *  @date    2016/04/02
 */
package com.example.democ.audio;

import android.annotation.TargetApi;
import android.media.MediaCodec;
import android.media.MediaCodecInfo;
import android.media.MediaFormat;
import android.os.Build;

import androidx.annotation.NonNull;

import com.example.democ.MainActivity;
import com.example.democ.hwencoder.AudioConfiguration;
import com.example.democ.hwencoder.AudioMediaCodec;
import com.example.democ.interfaces.CaptureDataListener;
import com.example.democ.interfaces.OutputEncodedDataListener;
import com.example.democ.interfaces.SingleInterface;

import java.io.IOException;
import java.nio.ByteBuffer;
import java.util.concurrent.ArrayBlockingQueue;
import java.util.concurrent.LinkedBlockingDeque;

@TargetApi(Build.VERSION_CODES.JELLY_BEAN)
public class AudioEncoder extends Thread {

    private static final String TAG = "TAG";

    private static final String DEFAULT_MIME_TYPE = "audio/mp4a-latm";

    private static final int MAX_BUFFER_SIZE = 100;

    private MediaCodec mMediaCodec;
    private OnAudioEncodedListener mAudioEncodedListener;
    private boolean mIsOpened = false;
    private long audioStartTime = 0L;
    private CaptureDataListener captureDataListener;
//    private ArrayBlockingQueue<byte[]> mAudioBufferList = new ArrayBlockingQueue<byte[]>(2000);

    private OutputEncodedDataListener mOutputListener;

    public void setOutputListener(OutputEncodedDataListener listener){
        this.mOutputListener = listener;
    }

    public void setCaptureDataListener(CaptureDataListener captureDataListener) {
        this.captureDataListener = captureDataListener;
    }


    public interface OnAudioEncodedListener {
        void onFrameEncoded(byte[] encoded, long presentationTimeUs);
    }

    @Override
    public void run() {
        super.run();
        while (mIsOpened){
            MediaCodec.BufferInfo bufferInfo = new MediaCodec.BufferInfo();
            int outputBufferIndex = mMediaCodec.dequeueOutputBuffer(bufferInfo, 1000);
            if (outputBufferIndex > 0) {
                ByteBuffer outputBuffer = mMediaCodec.getOutputBuffer(outputBufferIndex);
                outputBuffer.position(bufferInfo.offset);
                outputBuffer.limit(bufferInfo.offset + bufferInfo.size);
                // 写入混合流中
                if (outputBuffer != null) {
                    if (mOutputListener != null) mOutputListener.outputData(outputBuffer,bufferInfo);
                }
                mMediaCodec.releaseOutputBuffer(outputBufferIndex, false);
            } else if (outputBufferIndex == MediaCodec.INFO_OUTPUT_FORMAT_CHANGED) {
                if (mOutputListener != null) mOutputListener.outputFormatChanged(mMediaCodec.getOutputFormat());
            }
        }
    }

    private boolean open() {
        if (mIsOpened) {
            return true;
        }
        mMediaCodec = AudioMediaCodec.getAudioMediaCodec(AudioConfiguration.createDefault(), new SingleInterface<MediaCodec>() {
            @Override
            public void run(MediaCodec mediaCodec) {
//                config(mediaCodec);
            }
        });
        mMediaCodec.start();
        mIsOpened = true;
        log(TAG, "open audio encoder success !");
        return true;
    }

//    private void config(MediaCodec mediaCodec) {
//        mediaCodec.setCallback(new MediaCodec.Callback() {
//            @Override
//            public void onInputBufferAvailable(@NonNull MediaCodec codec, int index) {
//                byte[] input = mAudioBufferList.poll();
//                if (input == null) return;
//                if (audioStartTime == 0L) audioStartTime = System.nanoTime();
//                ByteBuffer inputBuffer = mMediaCodec.getInputBuffer(index);
//                inputBuffer.clear();
//                inputBuffer.put(input);
//                mMediaCodec.queueInputBuffer(index, 0, input.length,
//                        (System.nanoTime() - audioStartTime) / 1000, 0);
//            }
//
//            @Override
//            public void onOutputBufferAvailable(@NonNull MediaCodec codec, int index, @NonNull MediaCodec.BufferInfo info) {
//                ByteBuffer outputBuffer = mMediaCodec.getOutputBuffer(index);
//                outputBuffer.position(info.offset);
//                outputBuffer.limit(info.offset + info.size);
//                // 写入混合流中
//                if (outputBuffer != null) {
//                    if (mOutputListener != null) mOutputListener.outputData(outputBuffer,info);
//                }
//                mMediaCodec.releaseOutputBuffer(index, false);
//            }
//
//            @Override
//            public void onError(@NonNull MediaCodec codec, @NonNull MediaCodec.CodecException e) {
//                log("DemoC","some error when encode audio");
//                e.printStackTrace();
//            }
//
//            @Override
//            public void onOutputFormatChanged(@NonNull MediaCodec codec, @NonNull MediaFormat format) {
//                if (mOutputListener != null) mOutputListener.outputFormatChanged(mMediaCodec.getOutputFormat());
//            }
//        });
//    }

    public void startEncode() {
        open();
        start();
        audioStartTime = System.nanoTime();
    }

    public void close() {
        log(TAG, "close audio encoder +");
        if (!mIsOpened) {
            return;
        }
        mIsOpened = false;
        try {
            interrupt();
            join(1000);
        } catch (Exception e) {
            e.printStackTrace();
        }
        mMediaCodec.stop();
        mMediaCodec.release();
        mMediaCodec = null;
        log(TAG, "close audio encoder -");
    }

    public boolean isOpened() {
        return mIsOpened;
    }

    public void setAudioEncodedListener(OnAudioEncodedListener listener) {
        mAudioEncodedListener = listener;
    }

    public boolean encode(byte[] input) {
        if (!mIsOpened) {
            return false;
        }
        try {
            log("TAG","i'm encoding audio");
            int inputBufferIndex = mMediaCodec.dequeueInputBuffer(1000);
            if (inputBufferIndex >= 0) {
                ByteBuffer inputBuffer = mMediaCodec.getInputBuffer(inputBufferIndex);
                inputBuffer.clear();
                inputBuffer.put(input);
                mMediaCodec.queueInputBuffer(inputBufferIndex, 0, input.length,
                        (System.nanoTime() - audioStartTime) / 1000, 0);
            }
        }catch (Exception e){
            e.printStackTrace();
        }
        return true;
    }


    private void log(String tag, String str) {
        MainActivity.Companion.log(str);
    }
}

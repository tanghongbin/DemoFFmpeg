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
import android.util.Log;

import com.example.democ.MainActivity;

import java.io.IOException;
import java.nio.ByteBuffer;

@TargetApi(Build.VERSION_CODES.JELLY_BEAN)
public class AudioEncoder {

    private static final String TAG = AudioEncoder.class.getSimpleName();

    private static final String DEFAULT_MIME_TYPE = "audio/mp4a-latm";
    private static final int DEFAULT_CHANNEL_NUM = 1;
    private static final int DEFAULT_SAMPLE_RATE = 44100;
    private static final int DEFAULT_BITRATE = 128 * 1000; //AAC-LC, 64 *1024 for AAC-HE
    private static final int DEFAULT_PROFILE_LEVEL = MediaCodecInfo.CodecProfileLevel.AACObjectLC;
    private static final int DEFAULT_MAX_BUFFER_SIZE = 16384;

    private MediaCodec mMediaCodec;
    private OnAudioEncodedListener mAudioEncodedListener;
    private boolean mIsOpened = false;
    private Thread mThread;

    public interface OnAudioEncodedListener {
        void onFrameEncoded(byte[] encoded, long presentationTimeUs);
    }

    public boolean open() {
        if (mIsOpened) {
            return true;
        }
        return open(DEFAULT_SAMPLE_RATE, DEFAULT_CHANNEL_NUM, DEFAULT_BITRATE, DEFAULT_MAX_BUFFER_SIZE);
    }

    public boolean open(int samplerate, int channels, int bitrate, int maxBufferSize) {
        log(TAG, "open audio encoder: " + samplerate + ", " + channels + ", " + maxBufferSize);
        if (mIsOpened) {
            return true;
        }

        try {
            mMediaCodec = MediaCodec.createEncoderByType(DEFAULT_MIME_TYPE);
            MediaFormat format = new MediaFormat();
            format.setString(MediaFormat.KEY_MIME, DEFAULT_MIME_TYPE);
            format.setInteger(MediaFormat.KEY_CHANNEL_COUNT, channels);
            format.setInteger(MediaFormat.KEY_SAMPLE_RATE, samplerate);
            format.setInteger(MediaFormat.KEY_BIT_RATE, bitrate);
            format.setInteger(MediaFormat.KEY_AAC_PROFILE, DEFAULT_PROFILE_LEVEL);
            format.setInteger(MediaFormat.KEY_MAX_INPUT_SIZE, maxBufferSize);
            mMediaCodec.configure(format, null, null, MediaCodec.CONFIGURE_FLAG_ENCODE);
            mMediaCodec.start();
            mIsOpened = true;
        } catch (IOException e) {
            e.printStackTrace();
            return false;
        }
        log(TAG, "open audio encoder success !");
        return true;
    }

    public void startRunTask(){
        mThread = getRetrieveTask();
        mThread.start();
    }

    public void close() {
        log(TAG, "close audio encoder +");
        if (!mIsOpened) {
            return;
        }
        mIsOpened = false;
        mThread = null;
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

    public synchronized boolean encode(byte[] input, long presentationTimeUs) {
        Log.d(TAG, "encode: " + presentationTimeUs);
        if (!mIsOpened) {
            return false;
        }

        try {
            ByteBuffer[] inputBuffers = mMediaCodec.getInputBuffers();
            int inputBufferIndex = mMediaCodec.dequeueInputBuffer(1000);
            if (inputBufferIndex >= 0) {
                ByteBuffer inputBuffer = inputBuffers[inputBufferIndex];
                inputBuffer.clear();
                inputBuffer.put(input);
                mMediaCodec.queueInputBuffer(inputBufferIndex, 0, input.length, presentationTimeUs, 0);
            }
        } catch (Throwable t) {
            t.printStackTrace();
            return false;
        }
//        log(TAG, "encode -");
        return true;
    }
    private int trackId;

    private Thread getRetrieveTask(){
        return new Thread(new Runnable() {
            @Override
            public void run() {
                try {
                    while (isOpened()){
                        retrieve();
                    }
                }catch (Exception e){
                    e.printStackTrace();
                }
                log("","encode task finished");
            }
        });
    }

    private synchronized boolean retrieve() {
        if (!mIsOpened) {
            return false;
        }

        try {
            ByteBuffer[] outputBuffers = mMediaCodec.getOutputBuffers();
            MediaCodec.BufferInfo bufferInfo = new MediaCodec.BufferInfo();
            int outputBufferIndex = mMediaCodec.dequeueOutputBuffer(bufferInfo, 1000);
            if (outputBufferIndex >= 0) {
                ByteBuffer outputBuffer = outputBuffers[outputBufferIndex];
                outputBuffer.position(bufferInfo.offset);
                outputBuffer.limit(bufferInfo.offset + bufferInfo.size);
//                outputBuffer.position(bufferInfo.offset);
//                outputBuffer.limit(bufferInfo.offset + bufferInfo.size);
//                byte[] frame = new byte[bufferInfo.size];
//                outputBuffer.get(frame);
                // 写入混合流中
                if (MuxerManager.Companion.getInstance().isReady() && outputBuffer != null){
                    MuxerManager.Companion.getInstance().writeSampleData(trackId,outputBuffer,bufferInfo);
//                            log("视频写入数据2:$trackId   buffer:$byteBuffer")
                }
//                mAudioEncodedListener.onFrameEncoded(frame,bufferInfo.presentationTimeUs);
//                log(TAG,"encoder retrieve data buffer " + bufferInfo.size);
                mMediaCodec.releaseOutputBuffer(outputBufferIndex, false);
            } else if (outputBufferIndex == MediaCodec.INFO_OUTPUT_FORMAT_CHANGED){
                trackId = MuxerManager.Companion.getInstance().addTrack(mMediaCodec.getOutputFormat());
                MuxerManager.Companion.getInstance().start();
            }
        } catch (Throwable t) {
            t.printStackTrace();
            return false;
        }
        return true;
    }

    private void log(String tag,String str){
        MainActivity.Companion.log(str);
    }
}

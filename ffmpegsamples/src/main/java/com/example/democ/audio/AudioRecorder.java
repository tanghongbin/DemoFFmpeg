package com.testthb.democ.audio;/*
 *  COPYRIGHT NOTICE  
 *  Copyright (C) 2016, Jhuster <lujun.hust@gmail.com>
 *  https://github.com/Jhuster/Android
 *   
 *  @license under the Apache License, Version 2.0 
 *
 *  @file    AudioCapturer.java
 *  
 *  @version 1.0     
 *  @author  Jhuster
 *  @date    2016/03/10    
 */

import android.media.AudioRecord;
import android.util.Log;

import com.testthb.democ.hwencoder.AudioConfiguration;
import com.testthb.democ.utils.AudioUtils;

public class AudioRecorder {

    private static final String TAG = "AudioRecorder";

    private AudioRecord mAudioRecord;
    private int mMinBufferSize = 0;
	
    private Thread mCaptureThread; 	
    private boolean mIsCaptureStarted = false;
    private volatile boolean mIsLoopExit = false;

    private OnAudioFrameCapturedListener mAudioFrameCapturedListener;

    public interface OnAudioFrameCapturedListener {
        public void onAudioFrameCaptured(byte[] audioData, int ret);
    }	

    public boolean isCaptureStarted() {		
        return mIsCaptureStarted;
    }

    public void setOnAudioFrameCapturedListener(OnAudioFrameCapturedListener listener) {
        mAudioFrameCapturedListener = listener;
    }

    public boolean startCapture() {

        if (mIsCaptureStarted) {
            Log.e(TAG, "Capture already started !");
            return false;
        }
    
        mMinBufferSize = AudioUtils.getRecordBufferSize(AudioConfiguration.createDefault());
        if (mMinBufferSize == AudioRecord.ERROR_BAD_VALUE) {
            Log.e(TAG, "Invalid parameter !");
            return false;
        }
        Log.d(TAG , "getMinBufferSize = "+mMinBufferSize+" bytes !");
		
        mAudioRecord = AudioUtils.getAudioRecord(AudioConfiguration.createDefault());
        if (mAudioRecord.getState() == AudioRecord.STATE_UNINITIALIZED) {
    	    Log.e(TAG, "AudioRecord initialize fail !");
	    return false;
        }		

        mAudioRecord.startRecording();

        mIsLoopExit = false;
        mCaptureThread = new Thread(new AudioCaptureRunnable());
        mCaptureThread.start();

        mIsCaptureStarted = true;

        Log.d(TAG, "Start audio capture success !");

        return true;
    }

    public void stopCapture() {

        if (!mIsCaptureStarted) {
            return;
        }

        mIsLoopExit = true;		
        try {
            mCaptureThread.interrupt();
            mCaptureThread.join(1000);
        } 
        catch (InterruptedException e) {		
            e.printStackTrace();
        }

        if (mAudioRecord.getRecordingState() == AudioRecord.RECORDSTATE_RECORDING) {
            mAudioRecord.stop();						
        }

        mAudioRecord.release();		
	
        mIsCaptureStarted = false;
        mAudioFrameCapturedListener = null;

        Log.d(TAG, "Stop audio capture success !");
    }

    private class AudioCaptureRunnable implements Runnable {		
	
        @Override
        public void run() {

            while (!mIsLoopExit) {

                int fixLength = 4096;
                byte[] buffer = new byte[fixLength];

                int ret = mAudioRecord.read(buffer, 0, fixLength);
                if (ret == AudioRecord.ERROR_INVALID_OPERATION) {
                    Log.e(TAG , "Error ERROR_INVALID_OPERATION");
                } 
                else if (ret == AudioRecord.ERROR_BAD_VALUE) {
                    Log.e(TAG , "Error ERROR_BAD_VALUE");
                } 
                else { 
                    if (mAudioFrameCapturedListener != null) {
                        mAudioFrameCapturedListener.onAudioFrameCaptured(buffer,ret);
                    }   
//                    Log.d(TAG , "OK, Captured "+ret+" bytes !");
                }														
            }		
        }    
    }
}
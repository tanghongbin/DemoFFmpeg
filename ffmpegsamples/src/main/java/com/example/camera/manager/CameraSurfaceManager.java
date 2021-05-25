package com.example.camera.manager;

import android.graphics.Bitmap;
import android.graphics.BitmapFactory;
import android.graphics.ImageFormat;
import android.graphics.Rect;
import android.graphics.YuvImage;
import android.hardware.Sensor;
import android.hardware.SensorEvent;
import android.hardware.SensorEventListener;
import android.hardware.SensorManager;
import android.os.Handler;
import android.os.Looper;
import android.util.Log;
import android.widget.Toast;


import com.example.camera.Contacts;
import com.example.camera.listener.CameraPictureListener;
import com.example.camera.listener.CameraYUVDataListener;
import com.example.camera.util.CameraUtil;
import com.example.camera.util.SPUtil;
import com.example.common_base.DemoApplication;

import java.io.ByteArrayOutputStream;

import static android.content.Context.SENSOR_SERVICE;

/**
 * 作者：请叫我百米冲刺 on 2017/11/7 上午10:52
 * 邮箱：mail@hezhilin.cc
 */
@SuppressWarnings("deprecation")
public class CameraSurfaceManager implements SensorEventListener, CameraYUVDataListener {

    private CameraSurfaceView mCameraSurfaceView;
    public CameraUtil mCameraUtil;
    private boolean isTakingPicture;
    private boolean isRunning;
    private CameraPictureListener listener;


    private int scaleWidth;
    private int scaleHeight;
    private int cropStartX;
    private int cropStartY;
    private int cropWidth;
    private int cropHeight;

    //传感器需要，这边使用的是重力传感器
    private SensorManager mSensorManager;
    //第一次实例化的时候是不需要的
    private boolean mInitialized = false;
    private float mLastX = 0f;
    private float mLastY = 0f;
    private float mLastZ = 0f;

    public CameraSurfaceManager(CameraSurfaceView cameraSurfaceView) {
        mCameraSurfaceView = cameraSurfaceView;
        mCameraUtil = cameraSurfaceView.getCameraUtil();
        mCameraSurfaceView.setCameraYUVDataListener(this);
        mSensorManager = (SensorManager) DemoApplication.instance.getSystemService(SENSOR_SERVICE);
    }

    public void setCameraPictureListener(CameraPictureListener listener) {
        this.listener = listener;
    }

    public int changeCamera() {
        return mCameraSurfaceView.changeCamera();
    }

    public void takePicture() {
        if (isSupport()) {
            isTakingPicture = true;
        }
    }

    public void onResume() {
        //打开摄像头
        mCameraSurfaceView.openCamera();
        //注册加速度传感器
        mSensorManager.registerListener(this, mSensorManager.getDefaultSensor(Sensor.TYPE_ACCELEROMETER), SensorManager.SENSOR_DELAY_UI);
    }

    public void onStop() {
        //释放摄像头
        mCameraSurfaceView.releaseCamera();
        mSensorManager.unregisterListener(this);
    }

    @Override
    public void onCallback(final byte[] srcData) {
        //进行一次拍照
        if (isTakingPicture && !isRunning) {
            isTakingPicture = false;
            isRunning = true;
            new Thread(new Runnable() {

                @Override
                public void run() {
                    int windowWidth = mCameraUtil.getCameraWidth(), windowHeight = mCameraUtil.getCameraHeight();
                    final int morientation = mCameraUtil.getOrientation();
                    Log.d("TAG", "打印各个属性" + String.format("widowWidth:%d widowHeight:%d scaleWidth:%d scaleHeight:%d \n" +
                            "cropWidth:%d cropHeight:%d morientation:%d ", windowWidth, windowHeight, scaleWidth, scaleHeight, cropWidth, cropHeight, morientation));

                    //进行yuv数据的缩放，旋转镜像缩放等操作 original
                    final byte[] dstData = new byte[scaleWidth * scaleHeight * 3 / 2];
//                    YuvUtil.yuvCompress(srcData, mCameraUtil.getCameraWidth(), mCameraUtil.getCameraHeight(), dstData, scaleHeight, scaleWidth, 3, morientation, morientation == 270);
                    //进行yuv数据裁剪的操作
//                    final byte[] cropData = new byte[cropWidth * cropHeight * 3 / 2];
//                    YuvUtil.yuvCropI420(dstData, scaleWidth, scaleHeight, cropData, cropWidth, cropHeight, cropStartX, cropStartY);
//       //             这里将yuvi420转化为nv21，因为yuvimage只能操作nv21和yv12，为了演示方便，这里做一步转化的操作
                    final byte[] nv21Data = new byte[scaleWidth * scaleHeight * 3 / 2];
//                    YuvUtil.yuvI420ToNV21(dstData, scaleWidth, scaleHeight, nv21Data);
                      YuvImage yuvImage = new YuvImage(nv21Data, ImageFormat.NV21, scaleWidth, scaleHeight, null);
                    ByteArrayOutputStream fOut = new ByteArrayOutputStream();
                    yuvImage.compressToJpeg(new Rect(0, 0, scaleWidth, scaleHeight), 100, fOut);

                    // custom drop cut
//                    scaleWidth = 1000;
//                    scaleHeight = 1900;
//                    final byte[] dstData = new byte[scaleWidth * scaleHeight * 3 / 2];
//                    YuvUtil.yuvCompress(srcData, mCameraUtil.getCameraWidth(), mCameraUtil.getCameraHeight(), dstData, scaleHeight, scaleWidth, 3, morientation, morientation == 270);
//                    //进行yuv数据裁剪的操作
////                    final byte[] cropData = new byte[cropWidth * cropHeight * 3 / 2];
////                    YuvUtil.yuvCropI420(dstData, scaleWidth, scaleHeight, cropData, cropWidth, cropHeight, cropStartX, cropStartY);
////       //             这里将yuvi420转化为nv21，因为yuvimage只能操作nv21和yv12，为了演示方便，这里做一步转化的操作
//                    final byte[] nv21Data = new byte[scaleWidth * scaleHeight * 3 / 2];
//                    YuvUtil.yuvI420ToNV21(dstData, scaleHeight, scaleWidth, nv21Data);
//                    YuvImage yuvImage = new YuvImage(nv21Data, ImageFormat.NV21, scaleHeight, scaleWidth, null);
//                    ByteArrayOutputStream fOut = new ByteArrayOutputStream();
//                    yuvImage.compressToJpeg(new Rect(0, 0, scaleHeight, scaleWidth), 100, fOut);


                    // custom test

//                    final byte[] dstData = new byte[windowWidth * windowHeight * 3 / 2];
////                    YuvUtil.yuvCompress(srcData, mCameraUtil.getCameraWidth(), mCameraUtil.getCameraHeight(), dstData, scaleHeight, scaleWidth, 3, morientation, morientation == 270);
//                    YuvUtil.yuvNV21ToI420AndRotate(srcData, windowHeight, windowWidth, dstData, morientation);
//                    //进行yuv数据裁剪的操作
//                    final byte[] cropData = new byte[cropWidth * cropHeight * 3 / 2];
//                    YuvUtil.yuvCropI420(dstData, scaleWidth, scaleHeight, cropData, cropWidth, cropHeight, cropStartX, cropStartY);
////       //             这里将yuvi420转化为nv21，因为yuvimage只能操作nv21和yv12，为了演示方便，这里做一步转化的操作
//                    final byte[] nv21Data = new byte[cropWidth * cropHeight * 3 / 2];
//                    YuvUtil.yuvI420ToNV21(cropData, cropWidth, cropHeight, nv21Data);
//                    YuvImage yuvImage = new YuvImage(nv21Data, ImageFormat.NV21, cropWidth, cropHeight, null);
//                    ByteArrayOutputStream fOut = new ByteArrayOutputStream();
//                    yuvImage.compressToJpeg(new Rect(0, 0, cropWidth, cropHeight), 100, fOut);


                    //将byte生成bitmap
                    byte[] bitData = fOut.toByteArray();
                    final Bitmap bitmap = BitmapFactory.decodeByteArray(bitData, 0, bitData.length);
                    new Handler(Looper.getMainLooper()).post(new Runnable() {
                        @Override
                        public void run() {
                            if (listener != null) {
                                listener.onPictureBitmap(bitmap);
                            }
                            isRunning = false;
                        }
                    });
                }
            }).start();
        }
    }

    @Override
    public void onSensorChanged(SensorEvent event) {
        float x = event.values[0];
        float y = event.values[1];
        float z = event.values[2];

        if (!mInitialized) {
            mLastX = x;
            mLastY = y;
            mLastZ = z;
            mInitialized = true;
        }

        float deltaX = Math.abs(mLastX - x);
        float deltaY = Math.abs(mLastY - y);
        float deltaZ = Math.abs(mLastZ - z);

        if (mCameraSurfaceView != null && (deltaX > 0.6 || deltaY > 0.6 || deltaZ > 0.6)) {
            mCameraSurfaceView.startAutoFocus(-1, -1);
        }

        mLastX = x;
        mLastY = y;
        mLastZ = z;
    }

    @Override
    public void onAccuracyChanged(Sensor sensor, int accuracy) {

    }

    //主要是对裁剪的判断
    public boolean isSupport() {
        scaleWidth = (int) SPUtil.get(Contacts.SCALE_WIDTH, 720);
        scaleHeight = (int) SPUtil.get(Contacts.SCALE_HEIGHT, 1280);
        cropWidth = (int) SPUtil.get(Contacts.CROP_WIDTH, 720);
        cropHeight = (int) SPUtil.get(Contacts.CROP_HEIGHT, 720);
        cropStartX = (int) SPUtil.get(Contacts.CROP_START_X, 0);
        cropStartY = (int) SPUtil.get(Contacts.CROP_START_Y, 0);
        if (cropStartX % 2 != 0 || cropStartY % 2 != 0) {
            Toast.makeText(DemoApplication.instance, "裁剪的开始位置必须为偶数", Toast.LENGTH_SHORT).show();
            return false;
        }
        if (cropStartX + cropWidth > scaleWidth || cropStartY + cropHeight > scaleHeight) {
            Toast.makeText(DemoApplication.instance, "裁剪区域超出范围", Toast.LENGTH_SHORT).show();
            return false;
        }
        return true;
    }
}
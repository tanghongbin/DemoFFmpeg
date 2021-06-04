package com.example.democ.utils;

import android.util.Log;

import com.libyuv.LibyuvUtil;

public class JavaYuvConvertHelper {
    public static byte[] convert(byte[] nv21, int videoWidth, int videoHeight, int rotation, boolean isFrontCamera) {
        long start = System.currentTimeMillis();
        byte[] yuvI420 = new byte[nv21.length];
        byte[] tempYuvI420 = new byte[nv21.length];

        LibyuvUtil.convertNV21ToI420(nv21, yuvI420, videoWidth, videoHeight);
        long start2 = System.currentTimeMillis();
//        LibyuvUtil.compressI420(yuvI420, videoWidth, videoHeight, tempYuvI420, videoWidth, videoHeight, rotation, isFrontCamera);
        long start3 = System.currentTimeMillis();
//        Log.d("TAG","nv21-i420的花费时间:"+(start2 - start) + "压缩时间:" + (start3 - start2));
        return yuvI420;
    }

    public static void convertNv21ToNv12(byte[] nv21, byte[] dst, int videoWidth, int videoHeight, int rotation, boolean isFrontCamera) {
        long start = System.currentTimeMillis();
        byte[] yuvI420 = new byte[nv21.length];
        byte[] temp420p = new byte[nv21.length];

        LibyuvUtil.convertNV21ToI420(nv21, yuvI420, videoWidth, videoHeight);
        LibyuvUtil.compressI420(yuvI420, videoWidth, videoHeight, temp420p, videoWidth, videoHeight, rotation, isFrontCamera);
        LibyuvUtil.convertI420ToNV12(temp420p, dst, videoWidth, videoHeight);

//        Log.d("TAG", "nv21-nv12的花费时间:" + (System.currentTimeMillis() - start));
    }
}

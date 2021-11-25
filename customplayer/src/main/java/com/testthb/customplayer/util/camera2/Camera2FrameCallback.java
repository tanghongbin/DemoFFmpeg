package com.testthb.customplayer.util.camera2;

public interface Camera2FrameCallback {
    void onPreviewFrame(byte[] data, int width, int height);
    void onCaptureFrame(byte[] data, int width, int height);
}

package com.testthb.customplayer.activity;

import android.util.Log;

import java.util.HashMap;

public class TestString {
    public static void test(){

        HashMap<String,String> map = new HashMap<>();
        for (int i = 0; i < 16; i++) {
            map.put("key"+i,"value"+i);
        }
        map.put("keyvalue","asdfsd");
    }
}

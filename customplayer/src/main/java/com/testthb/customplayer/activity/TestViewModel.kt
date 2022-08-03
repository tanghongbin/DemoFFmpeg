package com.testthb.customplayer.activity

import androidx.lifecycle.MutableLiveData
import androidx.lifecycle.ViewModel

class TestViewModel : ViewModel() {
    val liveData = MutableLiveData<String>()
}
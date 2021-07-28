package com.example.avutils.utils

import java.lang.Exception

fun Any.runCatchException(block:() -> Unit){
    try {
        block()
    }catch (e:Exception){
        e.printStackTrace()
    }
}
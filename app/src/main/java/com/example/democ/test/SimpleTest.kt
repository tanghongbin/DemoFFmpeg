package com.example.democ.test

import kotlin.jvm.Throws

class SimpleTest : Cloneable {
    @Throws(CloneNotSupportedException::class)
    override fun clone(): Any {
        arrayListOf<String>()
        return super.clone()
    }
}
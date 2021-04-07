package com.example.democ.view

import android.app.Dialog
import android.content.Context
import android.os.Bundle
import android.view.ViewGroup
import android.view.Window
import android.widget.ArrayAdapter
import com.example.democ.R
import kotlinx.android.synthetic.main.dialog_siamples.*

class SampleDialog(context: Context,style:Int) : Dialog(context,style) {
    private val samples = arrayListOf<String>(
        "Triangle","Texture","TextureMapSample","GLFBOSample","LightSample", "Model3DSample"
    )
    var mConfirm:(Int) -> Unit = {}
    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        setContentView(R.layout.dialog_siamples)
        val params =  window.attributes
        params.width = ViewGroup.LayoutParams.MATCH_PARENT
        params.height = ViewGroup.LayoutParams.MATCH_PARENT
        window.attributes = params
        mListView.adapter = ArrayAdapter<String>(context,android.R.layout.simple_list_item_1,samples)
        mListView.setOnItemClickListener { parent, view, position, id ->
//            dismiss()
            mConfirm(getNumByStr(position))
        }
    }

    private fun getNumByStr(position: Int): Int {
        return when(samples[position]){
            "Triangle" -> 1
            "Texture" -> 2
            "TextureMapSample" -> 3
            "GLFBOSample" -> 10
            "LightSample" -> 11
            "Model3DSample" -> 12
            else -> 1
        }
    }
}
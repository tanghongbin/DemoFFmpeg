package com.example.democ.view

import android.app.Dialog
import android.content.Context
import android.os.Bundle
import android.view.ViewGroup
import android.view.Window
import android.widget.ArrayAdapter
import com.example.democ.R
import com.example.democ.utils.GlslDataCenter
import kotlinx.android.synthetic.main.dialog_siamples.*

class SampleDialog(context: Context,style:Int) : Dialog(context,style) {

    var mConfirm:(Int) -> Unit = {}
    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        setContentView(R.layout.dialog_siamples)
        val params =  window.attributes
        params.width = ViewGroup.LayoutParams.MATCH_PARENT
        params.height = ViewGroup.LayoutParams.MATCH_PARENT
        window.attributes = params
        val samples = ArrayList(GlslDataCenter.getSampleKeys())
        mListView.adapter = ArrayAdapter<String>(context,android.R.layout.simple_list_item_1,samples)
        mListView.setOnItemClickListener { _, _, position, _ ->
//            dismiss()
            mConfirm(GlslDataCenter.getTypeByName(samples[position]))
        }
    }
}
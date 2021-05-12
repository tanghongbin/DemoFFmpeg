package com.example.customplayer.adapter

import android.view.View
import android.view.ViewGroup
import android.widget.ImageView
import android.widget.TextView
import androidx.recyclerview.widget.RecyclerView
import com.example.common_base.utils.displayLocalImage
import com.example.common_base.utils.inflateView
import com.example.customplayer.R
import com.example.customplayer.bean.VideoInfo

class PlayerListAdapter : RecyclerView.Adapter<PlayerListHolder>() {
    private val mList = arrayListOf<VideoInfo>()

    fun addList(data:List<VideoInfo>){
        mList.addAll(data)
        notifyDataSetChanged()
    }
    override fun onCreateViewHolder(parent: ViewGroup, viewType: Int): PlayerListHolder {
        return PlayerListHolder(inflateView(R.layout.item_video_list))
    }

    override fun getItemCount(): Int {
        return mList.size
    }

    override fun onBindViewHolder(holder: PlayerListHolder, position: Int) {
        val item = mList[position]
        val image = holder.itemView.findViewById<ImageView>(R.id.mListImage)
        displayLocalImage(item.thumbnailData,image)
        holder.itemView.findViewById<TextView>(R.id.mName).text = item.title
        holder.itemView.findViewById<TextView>(R.id.mDuration).text = item.durationTime
    }

}

class PlayerListHolder(itemview:View) : RecyclerView.ViewHolder(itemview) {

}
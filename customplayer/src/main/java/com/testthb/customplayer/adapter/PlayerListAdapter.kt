package com.testthb.customplayer.adapter

import android.view.View
import android.view.ViewGroup
import android.widget.ImageView
import android.widget.TextView
import androidx.recyclerview.widget.RecyclerView
import com.testthb.common_base.utils.displayLocalImage
import com.testthb.common_base.utils.inflateView
import com.testthb.customplayer.R
import com.testthb.customplayer.bean.VideoInfo

class PlayerListAdapter : RecyclerView.Adapter<PlayerListHolder>() {
    private val mList = arrayListOf<VideoInfo>()
    private var mBlock:((Int,VideoInfo) -> Unit)? = null
    private var mLongBlock:((Int,VideoInfo) -> Unit)? = null

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
        holder.itemView.setOnClickListener {
            mBlock?.invoke(position,mList[position])
        }
    }

    fun setOnItemClickListener(block:(Int,VideoInfo) -> Unit){
        mBlock = block
    }
    fun setOnItemLongClickListener(block:(Int,VideoInfo) -> Unit){
        mLongBlock = block
    }

}

class PlayerListHolder(itemview:View) : RecyclerView.ViewHolder(itemview) {

}
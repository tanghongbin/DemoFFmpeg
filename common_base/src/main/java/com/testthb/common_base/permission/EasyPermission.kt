package com.ruiyi.mvvmlib.permission
import android.content.Context
import android.content.pm.PackageManager
import androidx.core.content.ContextCompat
import androidx.fragment.app.FragmentActivity
/**
 * Author: LeeJie
 * Date:   2020/6/16
 * Desc：  EasyPermission初始化使用
 */

object EasyPermission {
    /**
     * 初始化
     */
    fun init(activity: FragmentActivity) = PermissionCollection(activity)
    /**
     * 是否授权判断
     */
    fun isGranted(context: Context, permission: String) = ContextCompat.checkSelfPermission(context, permission) == PackageManager.PERMISSION_GRANTED

    /******************* 待扩展 ***********************************************************************/

    /**
     * 自定义权限提示对话框样式的功能。
     */
    internal fun customRequestDialog() = run { }
    /**
     * 悬浮窗权限适配
     */
    internal fun alertWindowRequest() = run { }
}


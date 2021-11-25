package com.ruiyi.mvvmlib.permission

import androidx.fragment.app.FragmentActivity

/**
 * Author: LeeJie
 * Date:   2020/6/16
 * Desc:   传递权限的特定作用域
 */

class PermissionCollection internal constructor(private val activity: FragmentActivity) {
    fun permissions(vararg permissions: String) = EasyPermissionBuilder(activity, permissions.toList())
}
package com.ruiyi.mvvmlib.permission

/**
 * Author: LeeJie
 * Date:   2020/6/16
 * Desc：  提供权限申请弹框解释能力
 */
class ExplainReasonScope(private val permissionBuilder: EasyPermissionBuilder) {
    fun showRequestReasonDialog(permissions: List<String>, message: String = "应用程序需要访问权限,您需要在下个弹窗中允许我们", positiveText: String = "去开启", negativeText: String? = null) {
        permissionBuilder.showHandlePermissionDialog(
                true,
                permissions,
                message,
                positiveText,
                negativeText)
    }
}

class ForwardToSettingsScope(private val permissionBuilder: EasyPermissionBuilder) {
    fun showForwardToSettingsDialog(permissions: List<String>, message: String = "您需要去应用程序设置当中手动开启权限", positiveText: String = "去开启", negativeText: String = "暂不开启") {
        permissionBuilder
                .showHandlePermissionDialog(
                        false,
                        permissions,
                        message,
                        positiveText,
                        negativeText)
    }
}
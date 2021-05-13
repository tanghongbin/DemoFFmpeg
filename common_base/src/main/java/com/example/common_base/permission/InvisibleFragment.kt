package com.ruiyi.mvvmlib.permission
import android.content.Intent
import android.content.pm.PackageManager
import android.util.Log
import androidx.fragment.app.Fragment
typealias RequestCallback = (allGranted: Boolean, grantedList: List<String>, deniedList: List<String>) -> Unit
typealias ExplainReasonCallback = ExplainReasonScope.(deniedList: MutableList<String>) -> Unit
typealias ExplainReasonCallback2 = ExplainReasonScope.(deniedList: MutableList<String>, beforeRequest: Boolean) -> Unit
typealias ForwardToSettingsCallback = ForwardToSettingsScope.(deniedList: MutableList<String>) -> Unit
const val TAG = "InvisibleFragment"
const val PERMISSION_CODE = 1
const val SETTINGS_CODE = 2
/**
 * Author: LeeJie
 * Date:   2020/6/16
 * Desc：  一个不可见Fragment用来处理系统回调
 */
class InvisibleFragment : Fragment() {

    private lateinit var permissionBuilder: EasyPermissionBuilder
    /**
     * 申请权限原因回调，可为null
     */
    private var explainReasonCallback: ExplainReasonCallback? = null
    /**
     * 申请权限原因回调，可为null
     */
    private var explainReasonCallback2: ExplainReasonCallback2? = null
    /**
     * 去设置回调
     */
    private var forwardToSettingsCallback: ForwardToSettingsCallback? = null
    /**
     * 申请权限回调
     */
    private lateinit var requestCallback: RequestCallback

    fun requestNow(builder: EasyPermissionBuilder, cb1: ExplainReasonCallback?, cb2: ExplainReasonCallback2?, cb3: ForwardToSettingsCallback?, cb4: RequestCallback, vararg permissions: String) {
        permissionBuilder = builder
        explainReasonCallback = cb1
        explainReasonCallback2 = cb2
        forwardToSettingsCallback = cb3
        requestCallback = cb4
        requestPermissions(permissions, PERMISSION_CODE)
    }

    override fun onRequestPermissionsResult(requestCode: Int, permissions: Array<String>, grantResults: IntArray) {
        if (requestCode == PERMISSION_CODE) {
            val grantedList = ArrayList<String>()
            val showReasonList = ArrayList<String>()
            val forwardList = ArrayList<String>()
            for ((index, result) in grantResults.withIndex()) {
                if (result == PackageManager.PERMISSION_GRANTED) {
                    grantedList.add(permissions[index])
                    permissionBuilder.deniedPermissions.remove(permissions[index])
                    permissionBuilder.permanentDeniedPermissions.remove(permissions[index])
                } else {
                    val shouldShowReason = shouldShowRequestPermissionRationale(permissions[index])
                    if (shouldShowReason) {
                        showReasonList.add(permissions[index])
                        permissionBuilder.deniedPermissions.add(permissions[index])
                    } else {
                        forwardList.add(permissions[index])
                        permissionBuilder.permanentDeniedPermissions.add(permissions[index])
                        permissionBuilder.deniedPermissions.remove(permissions[index])
                    }
                }
            }
            permissionBuilder.grantedPermissions.clear()
            permissionBuilder.grantedPermissions.addAll(grantedList)
            val allGranted = permissionBuilder.grantedPermissions.size == permissionBuilder.allPermissions.size
            if (allGranted) {
                requestCallback(true, permissionBuilder.allPermissions, listOf())
            } else {
                var goesToRequestCallback = true
                if ((explainReasonCallback != null || explainReasonCallback2 != null) && showReasonList.isNotEmpty()) {
                    goesToRequestCallback = false
                    explainReasonCallback2?.let {
                        permissionBuilder.explainReasonScope.it(showReasonList, false)
                    } ?:
                    explainReasonCallback?.let { permissionBuilder.explainReasonScope.it(showReasonList) }
                }

                else if (forwardToSettingsCallback != null && forwardList.isNotEmpty()) {
                    goesToRequestCallback = false
                    forwardToSettingsCallback?.let { permissionBuilder.forwardToSettingsScope.it(forwardList) }
                }

                if (goesToRequestCallback || !permissionBuilder.showDialogCalled) {
                    val deniedList = ArrayList<String>()
                    deniedList.addAll(permissionBuilder.deniedPermissions)
                    deniedList.addAll(permissionBuilder.permanentDeniedPermissions)
                    requestCallback(false, permissionBuilder.grantedPermissions.toList(), deniedList)
                }
            }
        }
    }

    override fun onActivityResult(requestCode: Int, resultCode: Int, data: Intent?) {
        super.onActivityResult(requestCode, resultCode, data)
        if (requestCode == SETTINGS_CODE) {
            if (::permissionBuilder.isInitialized) {
                permissionBuilder.requestAgain(permissionBuilder.forwardPermissions)
            } else {
                Log.w("EasyPermission", "未初始化permissionBuilder")
            }
        }
    }
}
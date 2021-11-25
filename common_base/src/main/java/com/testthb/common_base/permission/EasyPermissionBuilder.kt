package com.ruiyi.mvvmlib.permission
import android.app.AlertDialog
import android.content.Intent
import android.net.Uri
import android.provider.Settings
import androidx.fragment.app.FragmentActivity
/**
 * Author: LeeJie
 * Date:   2020/6/16
 * Desc：  EasyPermissionBuilder
 */

class EasyPermissionBuilder internal constructor(private val activity: FragmentActivity, internal val allPermissions: List<String>) {

    private var explainReasonCallback: ExplainReasonCallback? = null

    private var explainReasonCallback2: ExplainReasonCallback2? = null

    private var forwardToSettingsCallback: ForwardToSettingsCallback? = null

    private var requestCallback: RequestCallback? = null

    private var explainReasonBeforeRequest = false

    internal val explainReasonScope = ExplainReasonScope(this)

    internal val forwardToSettingsScope = ForwardToSettingsScope(this)

    internal var showDialogCalled = false

    internal val grantedPermissions = HashSet<String>()

    internal val deniedPermissions = HashSet<String>()

    internal val permanentDeniedPermissions = HashSet<String>()

    internal val forwardPermissions = ArrayList<String>()

    fun onExplainRequestReason(callback: ExplainReasonCallback): EasyPermissionBuilder {
        explainReasonCallback = callback
        return this
    }

    fun onExplainRequestReason(callback: ExplainReasonCallback2): EasyPermissionBuilder {
        explainReasonCallback2 = callback
        return this
    }

    fun onForwardToSettings(callback: ForwardToSettingsCallback): EasyPermissionBuilder {
        forwardToSettingsCallback = callback
        return this
    }

    fun explainReasonBeforeRequest(): EasyPermissionBuilder {
        explainReasonBeforeRequest = true
        return this
    }

    fun request(callback: RequestCallback) {
        requestCallback = callback
        val requestList = ArrayList<String>()
        for (permission in allPermissions) {
            if (EasyPermission.isGranted(activity, permission)) {
                grantedPermissions.add(permission)
            } else {
                requestList.add(permission)
            }
        }
        if (requestList.isEmpty()) {
            callback(true, allPermissions, listOf())
            return
        }
        if (explainReasonBeforeRequest && (explainReasonCallback != null || explainReasonCallback2 != null)) {
            explainReasonBeforeRequest = false
            deniedPermissions.addAll(requestList)
            explainReasonCallback2?.let {
                explainReasonScope.it(requestList, true)
            } ?:
            explainReasonCallback?.let { explainReasonScope.it(requestList) }
        } else {
            requestNow(allPermissions, callback)
        }
    }

    internal fun requestAgain(permissions: List<String>) {
        if (permissions.isEmpty()) {
            onPermissionDialogCancel()
            return
        }
        requestCallback?.let {
            val permissionSet = HashSet(grantedPermissions)
            permissionSet.addAll(permissions)
            requestNow(permissionSet.toList(), it)
        }
    }

    internal fun showHandlePermissionDialog(showReasonOrGoSettings: Boolean, permissions: List<String>, message: String, positiveText: String, negativeText: String? = null) {
        showDialogCalled = true
        val filteredPermissions = permissions.filter {
            !grantedPermissions.contains(it) && allPermissions.contains(it)
        }
        if (filteredPermissions.isEmpty()) {
            onPermissionDialogCancel()
            return
        }
        AlertDialog.Builder(activity).apply {
            setMessage(message)
            setCancelable(negativeText.isNullOrBlank())
            setPositiveButton(positiveText) { _, _ ->
                if (showReasonOrGoSettings) {
                    requestAgain(filteredPermissions)
                } else {
                    forwardToSettings(filteredPermissions)
                }
            }
            negativeText?.let {
                setNegativeButton(it) { _, _ ->
                    onPermissionDialogCancel()
                }
            }
            show()
        }
    }

    private fun requestNow(permissions: List<String>, callback: RequestCallback) {
        getInvisibleFragment().requestNow(this, explainReasonCallback, explainReasonCallback2, forwardToSettingsCallback, callback, *permissions.toTypedArray())
    }

    private fun getInvisibleFragment(): InvisibleFragment {
        val fragmentManager = activity.supportFragmentManager
        val existedFragment = fragmentManager.findFragmentByTag(TAG)
        return if (existedFragment != null) {
            existedFragment as InvisibleFragment
        } else {
            val invisibleFragment = InvisibleFragment()
            fragmentManager.beginTransaction().add(invisibleFragment, TAG).commitNow()
            invisibleFragment
        }
    }

    private fun forwardToSettings(permissions: List<String>) {
        forwardPermissions.addAll(permissions)
        val intent = Intent(Settings.ACTION_APPLICATION_DETAILS_SETTINGS)
        val uri = Uri.fromParts("package", activity.packageName, null)
        intent.data = uri
        getInvisibleFragment().startActivityForResult(intent, SETTINGS_CODE)
    }

    private fun onPermissionDialogCancel() {
        val deniedList = ArrayList<String>()
        deniedList.addAll(deniedPermissions)
        deniedList.addAll(permanentDeniedPermissions)
        requestCallback?.let {
            it(deniedList.isEmpty(), grantedPermissions.toList(), deniedList)
        }
    }
}
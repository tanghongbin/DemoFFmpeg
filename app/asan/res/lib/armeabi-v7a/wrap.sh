#!/system/bin/sh
HERE="$(cd "$(dirname "$0")" && pwd)"
export ASAN_OPTIONS=log_to_syslog=false,allow_user_segv_handler=1
export ASAN_ACTIVATION_OPTIONS=include_if_exists=/data/local/tmp/asan.options.b
export LD_PRELOAD=$HERE/libclang_rt.asan-arm-android.so
$@
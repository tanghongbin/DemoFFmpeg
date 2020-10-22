# For more information about using CMake with Android Studio, read the
# documentation: https://d.android.com/studio/projects/add-native-code.html

# Sets the minimum version of CMake required to build the native library.

cmake_minimum_required(VERSION 3.4.1)

# Creates and names a library, sets it as either STATIC
# or SHARED, and provides the relative paths to its source code.
# You can define multiple libraries, and CMake builds them for you.
# Gradle automatically packages shared libraries with your APK.

set(jnilibs ${CMAKE_SOURCE_DIR}/../../../libs)
# ffmpegrender player render
set(libName native-ffmpegrender)

include_directories(
        ffmpegsamples
        sample
        include
        utils
        render
        glm
        test)


file(GLOB src-files
        ${CMAKE_SOURCE_DIR}/${libName}.cpp
        ${CMAKE_SOURCE_DIR}/sample/*.cpp
        ${CMAKE_SOURCE_DIR}/ffmpegsamples/encode/*.cpp
        ${CMAKE_SOURCE_DIR}/ffmpegsamples/capturer/*.cpp
        ${CMAKE_SOURCE_DIR}/render/*.cpp
        ${CMAKE_SOURCE_DIR}/test/*.cpp
        ${CMAKE_SOURCE_DIR}/utils/*.cpp)



# Searches for a specified prebuilt library and stores the path as a
# variable. Because CMake includes system libraries in the search path by
# default, you only need to specify the name of the public NDK library
# you want to add. CMake verifies that the library exists before
# completing its build.

add_library( # Sets the name of the library.
        ${libName}

        # Sets the library as a shared library.
        SHARED

        # Provides a relative path to your source file(s).
        ${src-files})


add_library(avcodec SHARED IMPORTED )
set_target_properties(avcodec PROPERTIES IMPORTED_LOCATION ${jnilibs}/${ANDROID_ABI}/libavcodec.so)

add_library(avfilter SHARED IMPORTED )
set_target_properties(avfilter PROPERTIES IMPORTED_LOCATION ${jnilibs}/${ANDROID_ABI}/libavfilter.so)

add_library(avformat SHARED IMPORTED )
set_target_properties(avformat PROPERTIES IMPORTED_LOCATION ${jnilibs}/${ANDROID_ABI}/libavformat.so)

add_library(avutil SHARED IMPORTED )
set_target_properties(avutil PROPERTIES IMPORTED_LOCATION ${jnilibs}/${ANDROID_ABI}/libavutil.so)

add_library(swresample SHARED IMPORTED )
set_target_properties(swresample PROPERTIES IMPORTED_LOCATION ${jnilibs}/${ANDROID_ABI}/libswresample.so)

add_library(swscale SHARED IMPORTED )
set_target_properties(swscale PROPERTIES IMPORTED_LOCATION ${jnilibs}/${ANDROID_ABI}/libswscale.so)

find_library(log-lib log )

find_library(OpenSLES-lib OpenSLES)

set(third-party-libs
        avcodec
        avfilter
        avformat
        avutil
        swresample
        swscale)

# Specifies libraries CMake should link to your target library. You
# can link multiple libraries, such as libraries you define in this
# build script, prebuilt third-party libraries, or system libraries.

target_link_libraries(${libName}
        ${OpenSLES-lib}
        ${log-lib}
        ${third-party-libs}
        EGL
        android
        GLESv3)
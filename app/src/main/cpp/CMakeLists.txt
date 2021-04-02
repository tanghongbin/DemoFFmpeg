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
set(libName native-render)
set(libName2 native-ffmpegrender)
set(libName3 native-player)

#拉流-推流
set(RTMP ${CMAKE_SOURCE_DIR}/librtmp)

#get_filename_component(PARENT_DIR ${CMAKE_SOURCE_DIR} PATH)

#使用单个yuv库
#set(LIBYUV_DIR ${CMAKE_SOURCE_DIR}/../../../../libyuv-single)

include_directories(
        ffmpegsamples
        sample
        include
        utils
        opengles_samples
        render
        glm
        rtmppusher
        test
        librtmp/include
#        ${LIBYUV_DIR}/libs/${ANDROID_ABI}/include
)

#指定 rtmp 静态库文件目录
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -L${RTMP}/libs/${CMAKE_ANDROID_ARCH_ABI}")

file(GLOB src-files
        ${CMAKE_SOURCE_DIR}/${libName}.cpp
        ${CMAKE_SOURCE_DIR}/sample/*.cpp
        ${CMAKE_SOURCE_DIR}/sample/high_level/*.cpp
        ${CMAKE_SOURCE_DIR}/opengles_samples/*.cpp
        ${CMAKE_SOURCE_DIR}/ffmpegsamples/rtmp/*.cpp
        ${CMAKE_SOURCE_DIR}/ffmpegsamples/encode/*.cpp
        ${CMAKE_SOURCE_DIR}/ffmpegsamples/capturer/*.cpp
        ${CMAKE_SOURCE_DIR}/ffmpegsamples/filters/*.cpp
        ${CMAKE_SOURCE_DIR}/ffmpegsamples/swscale/*.cpp
        ${CMAKE_SOURCE_DIR}/ffmpegsamples/muxer/*.cpp
        ${CMAKE_SOURCE_DIR}/ffmpegsamples/io/*.cpp
        ${CMAKE_SOURCE_DIR}/render/*.cpp
        ${CMAKE_SOURCE_DIR}/test/*.cpp
        ${CMAKE_SOURCE_DIR}/utils/helpers/*.cpp
        ${CMAKE_SOURCE_DIR}/utils/*.cpp)

#第二个库
file(GLOB src-files2
        ${CMAKE_SOURCE_DIR}/${libName2}.cpp
        ${CMAKE_SOURCE_DIR}/sample/*.cpp
        ${CMAKE_SOURCE_DIR}/ffmpegsamples/rtmp/*.cpp
        ${CMAKE_SOURCE_DIR}/ffmpegsamples/encode/*.cpp
        ${CMAKE_SOURCE_DIR}/ffmpegsamples/capturer/*.cpp
        ${CMAKE_SOURCE_DIR}/ffmpegsamples/filters/*.cpp
        ${CMAKE_SOURCE_DIR}/ffmpegsamples/swscale/*.cpp
        ${CMAKE_SOURCE_DIR}/ffmpegsamples/muxer/*.cpp
        ${CMAKE_SOURCE_DIR}/ffmpegsamples/io/*.cpp
        ${CMAKE_SOURCE_DIR}/render/*.cpp
        ${CMAKE_SOURCE_DIR}/test/*.cpp
        ${CMAKE_SOURCE_DIR}/utils/helpers/*.cpp
        ${CMAKE_SOURCE_DIR}/utils/*.cpp)


file(GLOB src-files3
        ${CMAKE_SOURCE_DIR}/${libName3}.cpp
        ${CMAKE_SOURCE_DIR}/sample/*.cpp
        ${CMAKE_SOURCE_DIR}/ffmpegsamples/rtmp/*.cpp
        ${CMAKE_SOURCE_DIR}/ffmpegsamples/encode/*.cpp
        ${CMAKE_SOURCE_DIR}/ffmpegsamples/capturer/*.cpp
        ${CMAKE_SOURCE_DIR}/ffmpegsamples/filters/*.cpp
        ${CMAKE_SOURCE_DIR}/ffmpegsamples/swscale/*.cpp
        ${CMAKE_SOURCE_DIR}/ffmpegsamples/muxer/*.cpp
        ${CMAKE_SOURCE_DIR}/ffmpegsamples/io/*.cpp
        ${CMAKE_SOURCE_DIR}/render/*.cpp
        ${CMAKE_SOURCE_DIR}/test/*.cpp
        ${CMAKE_SOURCE_DIR}/utils/helpers/*.cpp
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


add_library( # Sets the name of the library.
        ${libName2}

        # Sets the library as a shared library.
        SHARED

        # Provides a relative path to your source file(s).
        ${src-files2})

add_library( # Sets the name of the library.
        ${libName3}

        # Sets the library as a shared library.
        SHARED

        # Provides a relative path to your source file(s).
        ${src-files3})



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

add_library(mp3 SHARED IMPORTED )
set_target_properties(mp3 PROPERTIES IMPORTED_LOCATION ${jnilibs}/${ANDROID_ABI}/libmp3lame.so)

add_library(x264 SHARED IMPORTED )
set_target_properties(x264 PROPERTIES IMPORTED_LOCATION ${jnilibs}/${ANDROID_ABI}/libx264.so)

#add_library(libyuv SHARED IMPORTED )
#set_target_properties(libyuv PROPERTIES IMPORTED_LOCATION ${jnilibs}/${ANDROID_ABI}/libyuv.so)

add_library(fdk-aac SHARED IMPORTED )
set_target_properties(fdk-aac PROPERTIES IMPORTED_LOCATION ${jnilibs}/${ANDROID_ABI}/libfdk-aac.so)

#add_library(libyuv
#        SHARED
#        IMPORTED)
#
#set_target_properties(libyuv
#        PROPERTIES IMPORTED_LOCATION
#        ${LIBYUV_DIR}/libs/${ANDROID_ABI}/libyuv.so)

# ass crypto expat fontconfig freetype fribidi ssl

add_library(crypto SHARED IMPORTED )
set_target_properties(crypto PROPERTIES IMPORTED_LOCATION ${jnilibs}/${ANDROID_ABI}/libcrypto.so)

add_library(expat SHARED IMPORTED )
set_target_properties(expat PROPERTIES IMPORTED_LOCATION ${jnilibs}/${ANDROID_ABI}/libexpat.so)

add_library(fontconfig SHARED IMPORTED )
set_target_properties(fontconfig PROPERTIES IMPORTED_LOCATION ${jnilibs}/${ANDROID_ABI}/libfontconfig.so)

add_library(freetype SHARED IMPORTED )
set_target_properties(freetype PROPERTIES IMPORTED_LOCATION ${jnilibs}/${ANDROID_ABI}/libfreetype.so)

add_library(fribidi SHARED IMPORTED )
set_target_properties(fribidi PROPERTIES IMPORTED_LOCATION ${jnilibs}/${ANDROID_ABI}/libfribidi.so)

add_library(ssl SHARED IMPORTED )
set_target_properties(ssl PROPERTIES IMPORTED_LOCATION ${jnilibs}/${ANDROID_ABI}/libssl.so)

add_library(ass SHARED IMPORTED )
set_target_properties(ass PROPERTIES IMPORTED_LOCATION ${jnilibs}/${ANDROID_ABI}/libass.so)

add_library(assimp SHARED IMPORTED )
set_target_properties(assimp PROPERTIES IMPORTED_LOCATION ${jnilibs}/${ANDROID_ABI}/libassimp.so)

find_library(log-lib log )

find_library(OpenSLES-lib OpenSLES)

set(third-party-libs
        avcodec
        avfilter
        avformat
        avutil
        swresample
        swscale
        mp3
        x264
#        libyuv
        ass
        crypto
        fdk-aac
        expat
        fontconfig
        freetype
        fribidi
        ssl
        rtmp
        assimp
        )

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

target_link_libraries(${libName2}
        ${OpenSLES-lib}
        ${log-lib}
        ${third-party-libs}
        EGL
        android
        GLESv3)

target_link_libraries(${libName3}
        ${OpenSLES-lib}
        ${log-lib}
        ${third-party-libs}
        EGL
        android
        GLESv3)

#if(USEASAN)
#    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fsanitize=address -fno-omit-frame-pointer")
#    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=address -fno-omit-frame-pointer")
#    set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -fsanitize=address")
#    set(CMAKE_STATIC_LINKER_FLAGS "${CMAKE_STATIC_LINKER_FLAGS} -fsanitize=address")
#endif(USEASAN)

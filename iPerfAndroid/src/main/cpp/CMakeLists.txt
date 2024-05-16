# For more information about using CMake with Android Studio, read the
# documentation: https://d.android.com/studio/projects/add-native-code.html

# Sets the minimum version of CMake required to build the native library.

cmake_minimum_required(VERSION 3.22.1)

# Declares and names the project.

project("iperfandroid")
ADD_DEFINITIONS(-DHAVE_CONFIG_H -UAF_INET6 -w -Wno-error=format-security)

# Creates and names a library, sets it as either STATIC
# or SHARED, and provides the relative paths to its source code.
# You can define multiple libraries, and CMake builds them for you.
# Gradle automatically packages shared libraries with your APK.

add_library( # Sets the name of the library.
        iperfandroid

        # Sets the library as a shared library.
        SHARED

        # Provides a relative path to your source file(s).
        iperf.c
        src/cjson.c
        src/dscp.c
        src/iperf_api.c
        src/iperf_auth.c
        src/iperf_client_api.c
        src/iperf_error.c
        src/iperf_locale.c
        src/iperf_sctp.c
        src/iperf_server_api.c
        src/iperf_tcp.c
        src/iperf_time.c
        src/iperf_udp.c
        src/iperf_util.c
        src/main.c
        src/net.c
        src/tcp_info.c
        src/timer.c
        src/units.c
        src/iperf.h
        src/iperf_api.h
        )

# Searches for a specified prebuilt library and stores the path as a
# variable. Because CMake includes system libraries in the search path by
# default, you only need to specify the name of the public NDK library
# you want to add. CMake verifies that the library exists before
# completing its build.

find_library( # Sets the name of the path variable.
        log-lib

        # Specifies the name of the NDK library that
        # you want CMake to locate.
        log)

# Specifies libraries CMake should link to your target library. You
# can link multiple libraries, such as libraries you define in this
# build script, prebuilt third-party libraries, or system libraries.

target_link_libraries( # Specifies the target library.
        iperfandroid
        android

        # Links the target library to the log library
        # included in the NDK.
        ${log-lib})
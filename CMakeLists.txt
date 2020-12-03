cmake_minimum_required(VERSION 3.4.1)
project(CDemo C)

set(CMAKE_C_STANDARD 11)

set(ffmpeg_header_dir ${CMAKE_SOURCE_DIR})
set(ffmpeg_dir /opt/ffmpeg)

include_directories(${ffmpeg_header_dir}/include)

add_library(avutil
        SHARED
        IMPORTED)

set_target_properties(avutil
        PROPERTIES IMPORTED_LOCATION
        ${ffmpeg_dir}/lib/libavutil.so)

link_libraries(avutil)

add_executable(CDemo main.c)
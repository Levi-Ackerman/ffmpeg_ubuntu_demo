cmake_minimum_required(VERSION 3.4.1)
project(CDemo)

set(CMAKE_C_STANDARD 11)

set(ffmpeg_header_dir ${CMAKE_SOURCE_DIR})
set(ffmpeg_dir /opt/ffmpeg)

include_directories(${ffmpeg_header_dir}/include)

#add_library(avutil
#        SHARED
#        IMPORTED)

#set_target_properties(avutil
#        PROPERTIES IMPORTED_LOCATION
#        ${ffmpeg_dir}/lib/libavutil.so)

link_directories(${ffmpeg_dir}/lib)

find_library(avutil
        avutil)

find_library(avformat
        avformat)

find_library(avcodec
        avcodec
)

find_library(avfilter
        avfilter
        )

find_library(avdevice
        avdevice
        )

link_libraries(avutil avformat avcodec avfilter avdevice)

FILE(GLOB SRC_C_FILES ${CMAKE_SOURCE_DIR}/src/*.c)

#message(hello_${SRC_C_FILES})

#aux_source_directory(src ./src)

add_executable(${PROJECT_NAME} ${SRC_C_FILES} include/common.h)
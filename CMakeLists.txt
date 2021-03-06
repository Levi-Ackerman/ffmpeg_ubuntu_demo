cmake_minimum_required(VERSION 3.4.1)
project(CDemo)


set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_FLAGS -std=c++11)

set(ffmpeg_dir /opt/ffmpeg)

include_directories(${CMAKE_SOURCE_DIR}/include/ffmpeg
        ${CMAKE_SOURCE_DIR}/include/custom
        ${CMAKE_SOURCE_DIR}/include
        ${CMAKE_SOURCE_DIR}/src)

include_directories(${CMAKE_SOURCE_DIR}/src/handler)

#add_library(avutil
#        SHARED
#        IMPORTED)

#set_target_properties(avutil
#        PROPERTIES IMPORTED_LOCATION
#        ${ffmpeg_dir}/lib/libavutil.so)

link_directories(${ffmpeg_dir}/lib)

#find_library(avutil
#        avutil)
#
#find_library(avformat
#        avformat)
#
#find_library(avcodec
#        avcodec
#        )
#
#find_library(avfilter
#        avfilter
#        )
#
#find_library(avdevice
#        avdevice
#        )
#find_library(sdl2
#        SDL2)

link_libraries(avutil avformat avcodec avfilter avdevice SDL2 swscale swresample pthread)

FILE(GLOB SRC_C_FILES ${CMAKE_SOURCE_DIR}/src/*.c ${CMAKE_SOURCE_DIR}/src/sdl/*.c)

FILE(GLOB SRC_CPP_FILES ${CMAKE_SOURCE_DIR}/src/*.cpp ${CMAKE_SOURCE_DIR}/src/player/*.cpp)

add_subdirectory(${CMAKE_SOURCE_DIR}/src/handler)
#
#aux_source_directory(${CMAKE_SOURCE_DIR}/src SRC_DIR)
#
#add_subdirectory(${CMAKE_SOURCE_DIR}/src/sdl)

#message(hello_${SRC_C_FILES})

#aux_source_directory(src ./src)

add_executable(${PROJECT_NAME} ${SRC_C_FILES} ${SRC_CPP_FILES})

find_package(OpenCV REQUIRED)
include_directories(${OpenCV_INCLUDE_DIRS})

target_link_libraries(${PROJECT_NAME} handler ${OpenCV_LIBS})

FIND_PACKAGE(OpenMP REQUIRED)
if (OPENMP_FOUND)
    message("OPENMP FOUND ${OpenMP_CXX_FLAGS}")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${OpenMP_C_FLAGS}")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${OpenMP_EXE_LINKER_FLAGS}")
endif ()
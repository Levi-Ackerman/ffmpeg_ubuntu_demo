add_definitions(-std=c++11)
include_directories(${CMAKE_SOURCE_DIR}/include/custom)
aux_source_directory(. src)
add_library(handler SHARED ${src})
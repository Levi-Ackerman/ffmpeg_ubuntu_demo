add_definitions(-std=c++11)
aux_source_directory(. src)
add_library(handler SHARED ${src})
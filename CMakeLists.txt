cmake_minimum_required(VERSION 3.19)
project(scheduler)

set(CMAKE_CXX_STANDARD 17)

include_directories(include)

aux_source_directory(src CPP_FILES)

add_executable(scheduler ${CPP_FILES})

target_link_libraries(scheduler m)

set(EXECUTABLE_OUTPUT_PATH "${CMAKE_BINARY_DIR}/bin" )
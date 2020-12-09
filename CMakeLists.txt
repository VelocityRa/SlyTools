cmake_minimum_required(VERSION 3.17)
project(SlyTools LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)

# For MinGW
if (MINGW)
    set(CMAKE_EXE_LINKER_FLAGS "-static-libgcc -static-libstdc++ -static")
    link_libraries(stdc++fs)
endif()

add_library(fs_utils STATIC src/fs.cpp src/fs.cpp src/fs.hpp)

link_libraries(fs_utils)

add_executable(sly_compressor src/sly_compressor.cpp)
add_executable(sly_unpacker src/sly_unpacker.cpp)
add_executable(sly_packer src/sly_packer.cpp)

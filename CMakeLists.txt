cmake_minimum_required(VERSION 3.17)
project(SlyTools LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# For MinGW
if (MINGW)
    set(CMAKE_EXE_LINKER_FLAGS "-static-libgcc -static-libstdc++ -static")
    link_libraries(stdc++fs)
endif()

add_library(fs_utils STATIC src/fs.cpp src/fs.hpp)
link_libraries(fs_utils)

add_executable(sly_decompressor src/sly_decompressor.cpp)
add_executable(sly_compressor src/sly_compressor.cpp)

add_executable(sly_1_unpacker src/sly_1_unpacker.cpp)
add_executable(sly_1_packer src/sly_1_packer.cpp)

add_executable(sly_2_3_unpacker src/sly_2_3_unpacker.cpp src/wal_toc.hpp)
add_executable(sly_2_3_packer src/sly_2_3_packer.cpp src/wal_toc.hpp)

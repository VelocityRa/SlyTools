cmake_minimum_required(VERSION 3.17)
project(SlyTools LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# For MinGW
if (MINGW)
    set(CMAKE_EXE_LINKER_FLAGS "-static-libgcc -static-libstdc++ -static")
    link_libraries(stdc++fs)
endif()

add_executable(sly_decompressor src/sly_decompressor.cpp)
add_executable(sly_compressor src/sly_compressor.cpp)

add_executable(sly_1_ps2_packer src/sly_1_ps2_packer.cpp)
add_executable(sly_1_ps2_unpacker src/sly_1_ps2_unpacker.cpp)

add_executable(sly_1_unpacker src/sly_1_unpacker.cpp)
add_executable(sly_1_packer src/sly_1_packer.cpp)

add_executable(sly_2_3_unpacker src/sly_2_3_unpacker.cpp)
add_executable(sly_2_3_packer src/sly_2_3_packer.cpp)

add_executable(stuart_little_3_unpacker src/stuart_little_3_unpacker.cpp)

add_executable(sly_1_ps2_baker src/sly_1_ps2_baker.cpp)

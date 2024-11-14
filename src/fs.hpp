#pragma once

#include "types.hpp"

#include <filesystem>
#include <fstream>
#include <istream>
#include <iterator>
#include <string_view>


#ifdef _MSC_VER
#define fopen64 fopen
#endif

namespace std::filesystem {

Buffer file_read(const std::string& filename, size_t read_size = 0) {
    const auto& filename_str = std::string(filename);

    std::ifstream file(filename_str, std::ios::in | std::ios::binary);
    if (!file.is_open())
        throw std::runtime_error("Failed to open: " + filename_str);

    // Don't eat new lines in binary
    file.unsetf(std::ios::skipws);

    static Buffer data;

    const bool read_whole_file = (read_size == 0);
    if (read_whole_file) {
        // Get file size
        read_size = std::filesystem::file_size(filename);

        data.reserve(read_size);

        // Read file
        data.insert(data.begin(), std::istream_iterator<u8>(file), std::istream_iterator<u8>());
        data.resize(read_size);
    } else {
        data.resize(read_size);

        file.read(reinterpret_cast<char*>(data.data()), read_size);
    }

    // #ifndef NDEBUG
    //     printf("Read %zu bytes\n", data.size());
    // #endif

    return data;
}
Buffer file_read(const std::filesystem::path& path, size_t read_size = 0) {
    return file_read(path.string(), read_size);
}

void file_write(const std::filesystem::path& path, const Buffer& data, bool trunc = true) {
    if (!trunc && std::filesystem::exists(path)) {
        printf("Skipping overwriting existing file %s\n", path.string().c_str());
        return;
    }

    const auto& path_str = path.string();

    std::ofstream file(path_str, std::ios::out | std::ios::binary);
    if (!file.is_open())
        throw std::runtime_error("Failed to open: " + path_str);

    // #ifndef NDEBUG
    //     printf("Writing %zu bytes\n", data.size());
    // #endif

    file.write(reinterpret_cast<const char*>(data.data()), data.size());
}

} // namespace fs

namespace fs = std::filesystem;

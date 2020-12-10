#include "fs.hpp"
#include "types.hpp"

#include <fstream>
#include <istream>
#include <iterator>
#include <string_view>

namespace filesystem {
Buffer file_read(std::string_view filename) {
    const auto& filename_str = std::string(filename);

    std::ifstream file(filename_str, std::ios::binary);
    if (!file.is_open())
        throw std::runtime_error("Failed to open: " + filename_str);

    // Don't eat new lines in binary
    // TODO: Needed?
    file.unsetf(std::ios::skipws);

    // Get size
    file.seekg(0, std::ios::end);
    std::streampos fileSize = file.tellg();
    file.seekg(0, std::ios::beg);

    // Reserve capacity
    Buffer data;
    data.reserve((u32)fileSize);

    // Read data
    data.insert(data.begin(), std::istream_iterator<u8>(file), std::istream_iterator<u8>());

#ifndef NDEBUG
    printf("Read %zu bytes\n", data.size());
#endif
    return data;
}

void file_write(std::string_view filename, const Buffer& data) {
    const auto& filename_str = std::string(filename);

    std::ofstream file(filename_str, std::ios::binary | std::ios::trunc);
    if (!file.is_open())
        throw std::runtime_error("Failed to open: " + filename_str);

    // Don't eat new lines in binary
    // TODO: Needed?
    file.unsetf(std::ios::skipws);

#ifndef NDEBUG
    printf("Writing %zu bytes\n", data.size());
#endif

    file.write(reinterpret_cast<const char*>(data.data()), data.size());
}
}  // namespace filesystem

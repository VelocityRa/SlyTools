#pragma once

#include "types.hpp"

#include <string_view>
#include <filesystem>

#ifdef _MSC_VER
#define fopen64 fopen
#endif

namespace filesystem {

Buffer file_read(std::string_view filename, size_t read_size = 0);
Buffer file_read(const std::filesystem::path& path, size_t read_size = 0);

void file_write(std::string_view filename, const Buffer& data, bool trunc = true);
void file_write(const std::filesystem::path& path, const Buffer& data, bool trunc = true);

} // namespace filesystem

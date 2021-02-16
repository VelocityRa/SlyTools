#pragma once

#include "types.hpp"

#include <string_view>

namespace filesystem {
Buffer file_read(std::string_view filename, size_t read_size = 0);

void file_write(std::string_view filename, const Buffer& data);
} // namespace filesystem

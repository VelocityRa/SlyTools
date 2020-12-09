#pragma once

#include "types.hpp"
#include <string_view>

namespace filesystem {

	Buffer file_read(std::string_view filename);

	void file_write(std::string_view filename, const Buffer &data);

}

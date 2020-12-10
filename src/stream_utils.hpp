#pragma once

#include "types.hpp"

#include <istream>
#include <ostream>

template <typename T>
void stream_read(std::istream& stream, T& data) {
    stream.read(reinterpret_cast<char*>(&data), sizeof(T));
}

template <typename T>
void stream_write(std::ostream& stream, const T& data) {
    stream.write(reinterpret_cast<const char*>(&data), sizeof(T));
}

void stream_write_buf(std::ostream& stream, const Buffer& data) {
    stream.write(reinterpret_cast<const char*>(data.data()), data.size());
}
#pragma once

#include "types.hpp"

#include <algorithm>
#include <cstring>
#include <istream>
#include <ostream>

class BufferStream {
   public:
    explicit BufferStream(const Buffer& buf) : buf(buf){};

    template <typename T>
    T read() {
        const T ret = *reinterpret_cast<const T*>(buf.data() + offset);
        offset += sizeof(T);
        return ret;
    }

    std::string read_string() {
        const auto ptr = reinterpret_cast<const char*>(buf.data() + offset);
        const auto str_size = std::strlen(ptr);

        std::string str;
        str.resize(str_size);
        std::copy(ptr, ptr + str_size, str.data());

        offset += str_size + 1;

        return str;
    }

    void align(u64 alignment) { offset += (-offset) & (alignment - 1); }

   private:
    const Buffer& buf{};

   public:
    u64 offset{};
};

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

template <typename T>
void stream_read(FILE* fp, T& data) {
    fread(&data, 1, sizeof(T), fp);
}

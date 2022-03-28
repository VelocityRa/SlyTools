#pragma once

#include "types.hpp"

#include <algorithm>
#include <cstring>
#include <cassert>
#include <istream>
#include <ostream>
#include <string>

class BufferStream {
   public:
    explicit BufferStream(const Buffer& buf) : buf(buf){};

    template <typename T>
    T read() {
        const T ret = *reinterpret_cast<const T*>(buf.data() + offset);
        offset += sizeof(T);
        offset_check(offset);
        return ret;
    }

    void skip(s64 offset_to_skip) {
        offset += offset_to_skip;
        offset_check(offset);
    }

    std::string read_string(u64 pos_to_read = u64(-1)) {
        const bool read_at_specific = (pos_to_read != u64(-1));

        u64 prev_offset;
        if (read_at_specific) {
            offset_check(pos_to_read);
            prev_offset = offset;
            offset = pos_to_read;
        }

        const auto ptr = reinterpret_cast<const char*>(buf.data() + offset);
        const auto str_size = std::strlen(ptr);

        std::string str;
        str.resize(str_size);
        std::copy(ptr, ptr + str_size, str.data());

        if (read_at_specific)
            offset = prev_offset;
        else {
            offset += str_size + 1;
            offset_check(offset);
        }

        return str;
    }

    void align(u64 alignment) { offset += (-offset) & (alignment - 1); }

   private:
    void offset_check(u64 new_offset) {
        if (new_offset > buf.size())
            throw std::runtime_error("Out of bounds!");
    }

   private:
    const Buffer& buf{};

   public:
    u64 offset{};
};

template <typename T>
void stream_read(std::istream& stream, T& data, size_t read_size = sizeof(T)) {
    stream.read(reinterpret_cast<char*>(&data), read_size);
}

void stream_skip(std::istream& stream, std::streamoff skip_offset) {
    stream.seekg(skip_offset, std::ios_base::cur);
}

template <typename T>
void stream_write(std::ostream& stream, const T& data, size_t write_size = sizeof(T)) {
    stream.write(reinterpret_cast<const char*>(&data), write_size);
}

void stream_write_buf(std::ostream& stream, const Buffer& data) {
    stream.write(reinterpret_cast<const char*>(data.data()), data.size());
}
void stream_write(std::ostream& stream, const Buffer& data) {
    stream_write_buf(stream, data);
}

void stream_write_null_char(std::ostream& stream) {
    constexpr char null_char ='\x00';
    stream.write(&null_char, 1);
}

void stream_write_str(std::ostream& stream, std::string_view sv) {
    stream.write(reinterpret_cast<const char*>(sv.data()), sv.size());
    stream_write_null_char(stream);
}

void stream_align(std::ostream& stream, u64 alignment) {
    static Buffer padding_buf;

    const auto pos = stream.tellp();
    const auto padding_size = (-pos) & (alignment - 1);

    padding_buf.resize(padding_size);
    stream_write_buf(stream, padding_buf);
}

void stream_pad_until(std::ostream& stream, u64 offset) {
    static Buffer padding_buf;

    stream.seekp(0, std::ios::end);
    const auto pos = stream.tellp();
    assert(offset >= pos);
    padding_buf.resize(offset - pos);
    stream_write(stream, padding_buf);
}
template <typename T>
void stream_read(FILE* fp, T& data, size_t read_size = sizeof(T)) {
    fread(&data, 1, read_size, fp);
}

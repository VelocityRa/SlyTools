#pragma once

#include "fs.hpp"
#include "types.hpp"

constexpr size_t CHUNK_SIZE = 0x2000;
constexpr size_t CHUNK_SIZE_MASK = CHUNK_SIZE - 1;

Buffer sly_decompress(const Buffer& data) {
    const auto input_size = data.size();

    Buffer output_data(CHUNK_SIZE * 2);
    u32 actual_output_data_size = 10 * input_size;
    Buffer actual_output_data(actual_output_data_size);

    u64 input_pos{};
    u64 output_pos{};

    u8 bits{};
    u16 src{};
    s16 ssize{};
    s16 offset{};
    s32 i{};
    u32 k{};

    while (input_pos < input_size) {
        bits = data[input_pos++];
        if (input_pos >= input_size)
            break;

        for (i = 0; i < 8; i++) {
            src = data[input_pos++];
            if (input_pos >= input_size)
                break;

            if (bits & 1) {
                output_data[output_pos++] = src;
                if (output_pos >= CHUNK_SIZE) {
                    output_pos &= CHUNK_SIZE_MASK;
                    memcpy(actual_output_data.data() + (k++ * CHUNK_SIZE), output_data.data(),
                           CHUNK_SIZE);
                }
            } else {
                src |= ((unsigned short)(data[input_pos++]) << 8);
                ssize = (src >> 13) + 2;
                offset = src & CHUNK_SIZE_MASK;
                while (ssize >= 0) {
                    --ssize;
                    output_data[output_pos++] = output_data[offset];
                    if (output_pos >= CHUNK_SIZE) {
                        output_pos &= CHUNK_SIZE_MASK;
                        memcpy(actual_output_data.data() + (k++ * CHUNK_SIZE), output_data.data(),
                               CHUNK_SIZE);
                    }
                    offset = (offset + 1) & CHUNK_SIZE_MASK;
                }
            }
            bits >>= 1;
        }
    }

    if (output_pos >= 0) {
        memcpy(actual_output_data.data() + (k++ * CHUNK_SIZE), output_data.data(), output_pos);
    }

    const size_t out_size = k * CHUNK_SIZE + output_pos;

    actual_output_data.resize(out_size);

    return actual_output_data;
}

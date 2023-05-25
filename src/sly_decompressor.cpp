#include "fs.hpp"
#include "types.hpp"

#include <fstream>
#include <iostream>

constexpr size_t CHUNK_SIZE = 0x2000;
constexpr size_t CHUNK_SIZE_MASK = CHUNK_SIZE - 1;

int main(int argc, const char** argv) {
    if (argc < 2) {
        std::cout << "Syntax: " << argv[0] << " "
                  << "<input_file>"
                  << "\n";
        return -1;
    }

    const std::string input_filename = argv[1];
    const auto input_data = filesystem::file_read(std::string_view(input_filename));
    const auto input_size = input_data.size();

    char output_data[CHUNK_SIZE*2];
    u32 actual_output_data_size = 10 * input_size;
    char* actual_output_data = new char[actual_output_data_size];

    u64 input_pos{};
    u64 output_pos{};

    u8 bits{};
    u16 src{};
    s16 ssize{};
    s16 offset{};
    s32 i{};
    u32 k{};

    while (input_pos < input_size) {
        bits = input_data[input_pos++];
        if (input_pos >= input_size)
            break;

        for (i = 0; i < 8; i++) {
            src = input_data[input_pos++];
            if (input_pos >= input_size)
                break;

            if (bits & 1) {
                output_data[output_pos++] = src;
                if (output_pos >= CHUNK_SIZE) {
                    output_pos &= CHUNK_SIZE_MASK;
                    memcpy(actual_output_data + (k++ * CHUNK_SIZE), output_data, CHUNK_SIZE);
                }
            } else {
                src |= ((unsigned short)(input_data[input_pos++]) << 8);
                ssize = (src >> 13) + 2;
                offset = src & CHUNK_SIZE_MASK;
                while (ssize >= 0) {
                    --ssize;
                    output_data[output_pos++] = output_data[offset];
                    if (output_pos >= CHUNK_SIZE) {
                        output_pos &= CHUNK_SIZE_MASK;
                        memcpy(actual_output_data + (k++ * CHUNK_SIZE), output_data, CHUNK_SIZE);
                    }
                    offset = (offset + 1) & CHUNK_SIZE_MASK;
                }
            }
            bits >>= 1;
        }
    }

    if (output_pos >= 0) {
        memcpy(actual_output_data + (k++ * CHUNK_SIZE), output_data, output_pos);
    }

    std::ofstream output(input_filename + ".dec", std::ios::binary | std::ios::out);
    size_t write_size = k * CHUNK_SIZE + output_pos;
    if (output.is_open()) {
        output.write(actual_output_data, write_size);
        output.close();
    } else {
        std::cerr << "Failed to open output file\n";
        return 1;
    }
    std::cout << "Finished!\n";
    delete[] actual_output_data;
    return 0;
}
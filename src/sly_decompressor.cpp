#include "fs.hpp"
#include "types.hpp"
#include "sly_decompress.hpp"

#include <fstream>
#include <iostream>

int main(int argc, const char** argv) {
    if (argc < 2) {
        std::cout << "Syntax: " << argv[0] << " "
                  << "<input_file>"
                  << "\n";
        return -1;
    }

    const std::string input_filename = argv[1];
    const auto input_data = fs::file_read(input_filename);

    const auto decompressed_data = sly_decompress(input_data);
    const auto output_filename = input_filename + ".dec";
    fs::file_write(output_filename, decompressed_data);

    return 0;
}
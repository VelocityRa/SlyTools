#include "fs.hpp"
#include "types.hpp"
#include "wac.hpp"

#include <algorithm>
#include <cstdio>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <iterator>
#include <stdexcept>

constexpr bool DEBUG_MODE = false;

#ifdef _MSC_VER
#define fopen64 fopen
#endif

int main(int argc, char* argv[]) {
    try {
        if (argc < 2)
            throw std::runtime_error(std::string(argv[0]) + " <input_file> [<output_dir>]");

        setvbuf(stdout, NULL, _IONBF, 0);

        const std::filesystem::path wac_path{ argv[1] };
        std::string wal_path_str{ argv[1] };
        wal_path_str.back() = 'L';

        std::filesystem::path output_path;
        if (argc < 3)
            output_path = wac_path.parent_path() / "extracted";
        else
            output_path = argv[2];
        std::filesystem::create_directory(output_path);

        const auto wac_fp = fopen64(wac_path.string().c_str(), "rb");

        if (wac_fp == NULL)
            throw std::runtime_error("Failed to open: " + wac_path.string());

        const auto wac_entries = parse_wac(wac_fp);

        const auto wal_fp = fopen64(wal_path_str.c_str(), "rb");

        Buffer file_data;
        for (const auto& entry : wac_entries) {
            const auto out_path = output_path / (entry.name + "_" + (char)entry.type);

            file_data.resize(entry.size);
            _fseeki64(wal_fp, entry.offset * SECTOR_SIZE, SEEK_SET);
            if (wal_fp == NULL)
                throw std::runtime_error("wal_fp == NULL");
            fread(file_data.data(), entry.size, 1, wal_fp);

            const auto out_fp = fopen64(out_path.string().c_str(), "wb");
            if (out_fp == NULL)
                throw std::runtime_error("out_fp == NULL");

            fwrite((const char*)file_data.data(), entry.size, 1, out_fp);

            if (DEBUG_MODE)
                printf("Wrote %s offset 0x%08llX size 0x%08llX\n", out_path.string().c_str(),
                       entry.offset * SECTOR_SIZE, file_data.size());

            fclose(out_fp);
        }

        fclose(wal_fp);
        fclose(wac_fp);
    } catch (const std::runtime_error& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        std::cerr << "errno: " << strerror(errno) << std::endl;
        return 1;
    }

    return 0;
}

#include "fs.hpp"
#include "types.hpp"
#include "wac.hpp"
#include "sly_decompress.hpp"

#include <algorithm>
#include <cstdio>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <iterator>
#include <stdexcept>

constexpr bool DEBUG_MODE = true;

using namespace ps2;

static Wac parse_wac_ps2(BufferStream& wac_data) {
    u32 wac_version = wac_data.read<u32>();
    if (wac_version != 0x02D1)
        throw std::runtime_error("WAC unsupported");

    Wac wac;

    const u32 world_name_c = wac_data.read<u32>();
    const u32 entry_c = wac_data.read<u32>();

    for (auto i = 0; i < world_name_c; i++)
        wac.world_names.push_back(wac_data.read_string());

    printf("Entry count: %d\n", entry_c);

    wac.file_names.resize(entry_c);
    for (auto& file_name : wac.file_names) {
        file_name = wac_data.read_string();
    }

    wac_data.align(4);
    
    wac.files.resize(entry_c);
    for (auto& file : wac.files) {
        file.sector = wac_data.read<u32>();
        file.size = wac_data.read<u32>();
    }

    return wac;
}

int main(int argc, char* argv[]) {
    try {
        if (argc < 2)
            throw std::runtime_error(std::string(argv[0]) + " <input_file> [<output_dir>]");

        const fs::path wac_path{ argv[1] };
        std::string wal_path_str{ argv[1] };
        wal_path_str.back() = 'L';

        auto wac_buf = fs::file_read(wac_path);

        crypt_ps2_wac(wac_buf);

        BufferStream wac_bufs(wac_buf);
        auto wac = parse_wac_ps2(wac_bufs);

        const auto wal_fp = fopen64(wal_path_str.c_str(), "rb");
        if (wal_fp == NULL)
            throw std::runtime_error("wal_fp == NULL");

        fs::path output_path;
        if (argc < 3)
            output_path = wac_path.parent_path() / "extracted";
        else
            output_path = argv[2];
        fs::create_directory(output_path);

        Buffer file_data;
        for (auto i = 0; i < wac.files.size(); i++) {
            auto& file = wac.files[i];

            file_data.resize(file.size);
            const u32 offset = file.sector * SECTOR_SIZE;
            _fseeki64(wal_fp, offset, SEEK_SET);
            fread(file_data.data(), file.size, 1, wal_fp);

            auto name = wac.file_names[i];

            for (auto& c : name)
                if (c == '\\')
                    c = ',';

            if (auto it = std::find(wac.world_names.cbegin(), wac.world_names.cend(), name);
                it != wac.world_names.end()) {
                name += ".brx";

                file_data = sly_decompress(file_data);
                file.size = file_data.size();
            }

            auto out_path = output_path / name;

            const auto out_fp = fopen64(out_path.string().c_str(), "wb");
            if (out_fp == NULL)
                throw std::runtime_error("out_fp == NULL");

            fwrite((const char*)file_data.data(), file.size, 1, out_fp);
            fclose(out_fp);
            
            if (DEBUG_MODE)
                printf("Wrote %s offset 0x%08llX size 0x%08llX\n",
                    out_path.string().c_str(), offset, file_data.size());
        }

        fclose(wal_fp);
    } catch (const std::runtime_error& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        std::cerr << "errno: " << strerror(errno) << std::endl;
        return 1;
    }

    return 0;
}

#include "fs.hpp"
#include "stream_utils.hpp"
#include "types.hpp"
#include "wac.hpp"

#include <filesystem>
#include <fstream>
#include <iterator>
#include <stdexcept>

constexpr bool DEBUG_MODE = true;
constexpr u32 LEVEL_END_PADDING_SIZE = 5 * 1024 * 1024;  // 5MB

using namespace ps2;

int main(int argc, char* argv[]) {
    try {
        if (argc <= 2)
            throw std::runtime_error(std::string(argv[0]) +
                                     " <input_dir> [<output_file_wac> <output_file_wal>]");

        const fs::path input_dir_path = argv[1];

        if (argc == 3)
            printf("Use either 1 or 3 arguments. Ignoring 2nd argument & extracting to working dir.\n");

        const auto wac_path = (argc == 4) ? argv[2] : input_dir_path / "SLY.WAC";
        std::ofstream wac_of(wac_path, std::ios::binary | std::ios::trunc);

        const auto wal_path = (argc == 4) ? argv[3] : input_dir_path / "SLY.WAL";
        std::ofstream wal_of(wal_path, std::ios::binary | std::ios::trunc);

        // Iterate dir, create Wac struct

        Wac wac;
        size_t file_count{};
        wac.world_names.push_back("splash");
        for (auto& p : fs::directory_iterator(input_dir_path)) {
            if (!p.is_regular_file())
                continue;

            const auto& path = p.path();

            auto name = path.lexically_relative(input_dir_path).string();
            wac.file_names.push_back(name);

            if (path.extension() == ".brx" && path.filename() != "splash.brx")
                wac.world_names.push_back(path.stem().string());

            file_count++;
        }

        // HACK: outmug.bnk in the Sly proto is written twice, once capitalized and once not. data is the // same
        wac.file_names.push_back("outmug.bnk");
        const u32 broken_idx =
            std::distance(wac.file_names.begin(),
                          std::find(wac.file_names.begin(), wac.file_names.end(), "OUTMUG.bnk"));
        file_count++;

        // Create WAC and WAL files

        stream_write(wac_of, (u32)0x2D1);

        stream_write(wac_of, (u32)wac.world_names.size());

        stream_write(wac_of, (u32)file_count);

        for (const auto& world_name : wac.world_names)
            stream_write_str(wac_of, world_name);

        for (auto name : wac.file_names) {
            for (auto& c : name)
                if (c == ',')
                    c = '\\';
            if (name.ends_with(".brx"))
                name.erase(name.size() - 4);
            stream_write_str(wac_of, name);
        }

        stream_align(wac_of, 4);

        for (auto i = 0; i < file_count; i++) {
            auto& name = wac.file_names[i];

            // Write file to WAL

            stream_align(wal_of, SECTOR_SIZE);
            const u32 wal_offs = (u32)wal_of.tellp();
            const u32 sector = wal_offs / SECTOR_SIZE;

            Buffer file_data = fs::file_read(input_dir_path / name);
            if (name.ends_with(".brx")) // For injecting more stuff to it
                file_data.insert(file_data.end(), LEVEL_END_PADDING_SIZE, 0xCD);

            const u32 size = (u32)file_data.size();

            stream_write_buf(wal_of, file_data);

            // Write entry to WAC

            stream_write(wac_of, sector);
            stream_write(wac_of, size);

            if (DEBUG_MODE)
                printf("Writing %s at sector 0x%X (addr 0x%X) size 0x%X\n",
                    name.c_str(), sector, sector * SECTOR_SIZE, size);
            else
                printf(".");
        }

        if (!DEBUG_MODE)
            printf(" done.\n");

    } catch (const std::runtime_error& e) {
        printf("Error: %s\n", e.what());
        return 1;
    }

    return 0;
}

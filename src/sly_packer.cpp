#include "types.hpp"
#include "fs.hpp"
#include "stream_utils.hpp"
#include "wac.hpp"

#include <iterator>
#include <fstream>
#include <stdexcept>
#include <filesystem>

int main(int argc, char *argv[]) {
    try {
        if (argc <= 2)
            throw std::runtime_error(std::string(argv[0]) + " <input_dir> <output_path>");

        const std::filesystem::path input_dir_path = argv[1];
        const std::filesystem::path output_dir_path = argv[2];

        const auto wac_path = output_dir_path / "SLY.WAC";
        std::ofstream wac_ofs(wac_path, std::ios::binary | std::ios::trunc);

        const auto wal_path = output_dir_path / "SLY.WAL";
        std::ofstream wal_ofs(wal_path, std::ios::binary | std::ios::trunc);

        u32 wal_size{};
        u64 wal_next_sector_offset{};

        u32 wac_entry_count{};

        stream_write(wac_ofs, (u32) 0);

#ifdef NDEBUG
        printf("Packing files");
#endif

        for (auto &p : std::filesystem::recursive_directory_iterator(input_dir_path)) {
            if (!p.is_regular_file()) {
                continue;
            }
            const auto full_path_str = p.path().string();
            const auto name_str = p.path().filename().string();

#ifndef NDEBUG
            printf("Reading %s\n", name_str.c_str());
#else
            printf(".");
#endif

            WACEntry wac_entry;

            wac_entry.type = (WACType) name_str.back();
            wac_entry.name = name_str.substr(0, name_str.size() - 2);
            wac_entry.data = filesystem::file_read(full_path_str);
            wac_entry.size = wac_entry.data.size();
            wac_entry.offset = wal_next_sector_offset;

            wal_next_sector_offset += (wac_entry.size + SECTOR_SIZE - 1) / SECTOR_SIZE;

            // Write entry to WAC

            const auto name_size = wac_entry.name.size();
            wac_ofs.write(wac_entry.name.c_str(), name_size);
            Buffer padding0;
            padding0.resize(WAC_ENTRY_NAME_LEN - name_size);
            stream_write_buf(wac_ofs, padding0);
            stream_write(wac_ofs, wac_entry.type);
            stream_write(wac_ofs, wac_entry.offset);
            stream_write(wac_ofs, wac_entry.size);

            // Write entry to WAL

            stream_write_buf(wal_ofs, wac_entry.data);
            wal_size += wac_entry.size;

            const auto padding_len = (wal_next_sector_offset * SECTOR_SIZE) - wal_size;
            Buffer padding1;
            padding1.resize(padding_len);
            stream_write_buf(wal_ofs, padding1);
            wal_size += padding_len;

            ++wac_entry_count;
        }

#ifdef NDEBUG
        printf(" done.\n");
#endif

        wac_ofs.seekp(0);
        stream_write(wac_ofs, wac_entry_count);

    } catch (const std::runtime_error &e) {
        printf("Error: %s\n", e.what());
        return 1;
    }

    return 0;
}
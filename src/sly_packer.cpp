#include "fs.hpp"
#include "stream_utils.hpp"
#include "types.hpp"
#include "wac.hpp"

#include <filesystem>
#include <fstream>
#include <iterator>
#include <stdexcept>

constexpr bool DEBUG_MODE = false;

int main(int argc, char* argv[]) {
    try {
        if (argc <= 2)
            throw std::runtime_error(std::string(argv[0]) +
                                     " <input_dir> [<output_file_wac> <output_file_wal>]");

        const std::filesystem::path input_dir_path = argv[1];

        if (argc == 3)
            printf("Use either 1 or 3 arguments. Ignoring 2nd argument & extracting to working dir.\n");

        const auto wac_path = (argc == 4) ? argv[2] : input_dir_path / "SLY.WAC";
        std::ofstream wac_ofs(wac_path, std::ios::binary | std::ios::trunc);

        const auto wal_path = (argc == 4) ? argv[3] : input_dir_path / "SLY.WAL";
        std::ofstream wal_ofs(wal_path, std::ios::binary | std::ios::trunc);

        u32 wal_size{};
        u32 wal_next_sector_offset{};
        u32 wac_entry_count{};

        // We'll write the entry count here later
        stream_write(wac_ofs, (u32)0);

        if (!DEBUG_MODE)
            printf("Packing files\n");

        for (auto& p : std::filesystem::directory_iterator(input_dir_path)) {
            if (!p.is_regular_file()) {
                continue;
            }
            const auto full_path_str = p.path().string();
            const auto name_str = p.path().filename().string();

            WACEntry wac_entry;

            wac_entry.type = (WACType)name_str.back();
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

            if (DEBUG_MODE)
                printf("Writing %s at offset 0x%X size 0x%X pad 0x%X\n", name_str.c_str(), wac_entry.offset, wac_entry.size, padding_len);
            else
                printf(".");

            ++wac_entry_count;
        }

        if (!DEBUG_MODE)
            printf(" done.\n");

        wac_ofs.seekp(0);
        stream_write(wac_ofs, wac_entry_count);

    } catch (const std::runtime_error& e) {
        printf("Error: %s\n", e.what());
        return 1;
    }

    return 0;
}

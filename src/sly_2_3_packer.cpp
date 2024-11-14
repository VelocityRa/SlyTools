#include "fs.hpp"
#include "stream_utils.hpp"
#include "string_utils.h"
#include "types.hpp"
#include "wal_toc.hpp"

#include <cassert>
#include <fstream>
#include <iterator>
#include <map>
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>

constexpr bool DEBUG_MODE = false;

constexpr size_t SECTOR_SIZE = 2048;

/*
local uint32 file_count = 0;
local uint32 i = 0;
local uint32 j = 0;

typedef struct TOCFileEntry_t (string name, char type) {
    uint32 offset;
    uint32 size;

    Printf("%c %s %X %X\n", type, name, offset, size);
} TOCFileEntry <open=true>;

typedef struct TOCEntry_t {
    string name;

    string types;

    local uint32 types_count = sizeof(types) - 1;

    if (FTell() & 0x3)
        align _pad(4);

    for (j = 0; j < types_count; j++)
        TOCFileEntry file (name, types[j]);

    file_count += types_count;
} TOCEntry <optimize=false>;

//
// Object allocation
//

uint32 unk0;
uint32 entry_count;
TOCEntry entries[entry_count];
 */

struct Wal2TocFileEntry {
    u32 offset;
    u32 size;
    char type;

    fs::path path;
};

using Wal2Toc = std::map<std::string, std::vector<Wal2TocFileEntry>>;

int main(int argc, char* argv[]) {
#if 0
    FILE* wal_out_fp = fopen64(argv[2], "r+b");
    if (wal_out_fp == NULL) {
        perror("Error opening file: ");
        return -1;
    }
    _fseeki64(wal_out_fp, 0x6B090000, SEEK_SET);

    const auto data = fs::file_read(argv[1]);
    auto wrote = fwrite(data.data(), 1, data.size(), wal_out_fp);
    printf("data size 0x%X wrote 0x%X\n", data.size(), wrote);
    fclose(wal_out_fp);
    return 0;
#else

    try {
        if (argc <= 2)
            throw std::runtime_error(std::string(argv[0]) + " <input_dir> [<output_file_wal>]");

        const fs::path input_dir_path = argv[1];

        const auto output_wal_path = (argc == 3) ? argv[2] : input_dir_path / "../SLY2_custom.WAL";
        std::ofstream wal_ofs(output_wal_path, std::ios::binary | std::ios::trunc);

        u32 wal_next_sector_offset{ MAX_TOC_SIZE / SECTOR_SIZE };

        // Unk field
        stream_write(wal_ofs, (u32)0x2B6);

        // We'll write the entry count here later
        stream_write(wal_ofs, (u32)0);

        printf("Consolidating files...\n");

        Wal2Toc wal_toc_entries;

        for (auto& p : fs::directory_iterator(input_dir_path)) {
            if (!p.is_regular_file()) {
                continue;
            }
            const auto full_path_str = p.path().string();
            const auto path_str = p.path().filename().string();
            const auto path_str_tokens = util::string::split(path_str, ".");

            const char file_type = path_str_tokens[1].back();
            const auto file_name = path_str_tokens[0];

            //            if (file_name != "jb_intro")
            //                continue;

            //            printf("%s\t%c\n", file_name.c_str(), file_type);

            Wal2TocFileEntry entry;
            entry.path = p;
            entry.size = fs::file_size(p);
            entry.type = file_type;
            entry.offset = wal_next_sector_offset;

            if (file_name == "f_nightclub_exterior" && file_type == 'Z')
                printf("offset: %X\n", entry.offset*SECTOR_SIZE);

            wal_next_sector_offset += (entry.size + SECTOR_SIZE - 1) / SECTOR_SIZE;

            wal_toc_entries[file_name].emplace_back(entry);
        }

        printf("Writing TOC...\n");

        for (const auto& entry_it : wal_toc_entries) {
            const auto& name = entry_it.first;
            const auto& entry = entry_it.second;

            stream_write_str(wal_ofs, name);

            for (const auto& file_entry : entry)
                stream_write(wal_ofs, file_entry.type);
            stream_write_null_char(wal_ofs);

            stream_align(wal_ofs, 4);

            for (const auto& file_entry : entry) {
                stream_write(wal_ofs, file_entry.offset);
                stream_write(wal_ofs, file_entry.size);
            }
        }

        printf("Encrypting TOC...\n");

        stream_pad_until(wal_ofs, MAX_TOC_SIZE);

        wal_ofs.seekp(4);
        stream_write(wal_ofs, wal_toc_entries.size());

        // Read back TOC data, encrypt it, and write it again (a bit dumb)
        wal_ofs.flush();
        auto wal_toc_data = fs::file_read(output_wal_path);
        assert(wal_toc_data.size() == MAX_TOC_SIZE);
        wal_toc_crypt(wal_toc_data.data());
        wal_ofs.seekp(0, std::ios::beg);
        stream_write(wal_ofs, wal_toc_data);

        printf("Sorting file entries...\n");

        std::map<u32, Wal2TocFileEntry> wal_toc_entries_flat_ordered;

        for (const auto& entries_it : wal_toc_entries) {
            const auto& entries = entries_it.second;
            for (const auto& entry : entries) {
                wal_toc_entries_flat_ordered[entry.offset] = entry;
            }
        }

        printf("Writing file data...\n");

        for (const auto& entry_it : wal_toc_entries_flat_ordered) {
            const auto& entry = entry_it.second;
//            printf("%08X %08X %c %s\n", entry.offset,entry.size, entry.type, entry.path.string().c_str());

            stream_pad_until(wal_ofs, entry.offset * SECTOR_SIZE);

            const auto data = fs::file_read(entry.path);
            stream_write(wal_ofs, data);
        }

        printf("\nDone writing %zu files.\n", wal_toc_entries_flat_ordered.size());

    } catch (const std::runtime_error& e) {
        printf("Error: %s\n", e.what());
        return 1;
    }

#endif

    return 0;
}

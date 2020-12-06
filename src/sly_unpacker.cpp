#include "types.hpp"
#include "fs.hpp"
#include "stream_utils.hpp"
#include "wac.hpp"

#include <algorithm>
#include <fstream>
#include <iterator>
#include <stdexcept>
#include <cstring>

WACEntries parse_wac(std::istream &wac_data) {
    u32 entry_count{};
    stream_read(wac_data, entry_count);
    printf("Entry count: %d\n", entry_count);

    WACEntries wac_entries;
    wac_entries.reserve(entry_count);

    for (auto i = 0; i < entry_count; ++i) {
        WACEntry entry;

        char name[WAC_ENTRY_NAME_LEN]{};
        stream_read(wac_data, name);
        entry.name.resize(std::strlen(name));
        std::copy(name, name + entry.name.size(), entry.name.begin());

        WACType type{};
        stream_read(wac_data, type);
        entry.type = type;

        u32 offset{};
        stream_read(wac_data, offset);
        entry.offset = offset;

        u32 size{};
        stream_read(wac_data, size);
        entry.size = size;

        wac_entries.push_back(entry);
    }

    return wac_entries;
}

int main(int argc, char *argv[]) {
    try {
        if (argc <= 2)
            throw std::runtime_error(std::string(argv[0]) + " <input_file> [<output_dir>]");

        const std::string wac_path = argv[1];
        const std::string output_path = (argc == 3) ? argv[2] : wac_path + "/extracted";

        std::string wal_path = wac_path;
        wal_path.back() = 'L';

        std::ifstream wac_ifs(wac_path, std::ios::binary);
        if (!wac_ifs.is_open())
            throw std::runtime_error("Failed to open: " + wac_path);
        wac_ifs.unsetf(std::ios::skipws);

        const auto wac_entries = parse_wac(wac_ifs);

        // TODO: This is super slow but seeking didn't seem to work (see commented code below)
        const auto wal_data = filesystem::file_read(wal_path);
//        std::ifstream wal_ifs(wal_path, std::ios::binary);
//        wal_ifs.unsetf(std::ios::skipws);

        for (const auto &entry : wac_entries) {
            const auto out_path = output_path + "/" + entry.name + "_" + (char) entry.type;

//            if (entry.offset==0x00000469)
//                assert(false);

//            Buffer file_data;
//            file_data.resize(entry.size);
//            wal_ifs.seekg(entry.offset * SECTOR_SIZE);
//            wal_ifs.read(reinterpret_cast<char *>(file_data.data()), entry.size);

            const unsigned char *start = wal_data.data() + entry.offset * SECTOR_SIZE;
//            std::copy(start, start + entry.size, file_data.begin());

            std::ofstream out_ofs(out_path, std::ios::binary | std::ios::trunc);
//            out_ofs.write(reinterpret_cast<const char *>(file_data.data()), entry.size);
            out_ofs.write(reinterpret_cast<const char *>(start), entry.size);
        }

    } catch (const std::runtime_error &e) {
        printf("Error: %s\n", e.what());
        return 1;
    }

    return 0;
}

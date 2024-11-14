#include "fs.hpp"
#include "stream_utils.hpp"
#include "types.hpp"

#include <algorithm>
#include <cstdio>
#include <fstream>
#include <iostream>
#include <iterator>
#include <sstream>
#include <stdexcept>
#include <vector>

constexpr bool DEBUG_MODE = false;
constexpr size_t SECTOR_SIZE = 2048;

struct InfEntry {
    u32 id{};
    std::string name;
    u32 offset{};  // Sector offset in WAD
    u32 size{};
};

struct Inf {
    u32 entry_count;
    std::vector<InfEntry> entries;
    u32 id_arr_offset{};
};

static Inf parse_inf(const Buffer& inf_data) {
    Inf inf;
    BufferStream s(inf_data);

    s.skip(2 * 4);
//    inf.entry_count = s.read<u32>() * 0x10 / 0xC;
    inf.entry_count = s.read<u32>();
    s.skip(2 * 4);
    inf.id_arr_offset = s.read<u32>();
    // TODO
    //    inf.entry_count = 0x10C4; // GARDEN
    // inf.entry_count = 97;  // GLOBAL
    inf.entries.reserve(inf.entry_count);

    for (auto i = 0; i < inf.entry_count; ++i) {
        InfEntry entry;

        entry.offset = s.read<u32>();
        entry.size = s.read<u32>();
        u32 str_offset = s.read<u32>();
        entry.name = s.read_string(str_offset);

        inf.entries.push_back(entry);
    }

    s.offset = inf.id_arr_offset;
    for (InfEntry& entry : inf.entries) {
        entry.id = s.read<u32>();
    }

    return inf;
}

template <typename T>
std::string pad_int(T num, size_t pad = sizeof(T) * 2) {
    std::ostringstream out;
    out << std::internal << std::setfill('0') << std::setw(pad) << num;
    return out.str();
}

int main(int argc, char* argv[]) {
    try {
#if 0
        if (argc < 2)
            throw std::runtime_error(std::string(argv[0]) + " <input_file> [<output_dir>]");

        const fs::path inf_path{ std::string(argv[1]) + ".INF" };
        const fs::path wad_path{ std::string(argv[1]) + ".WAD" };
#else
        const auto path_base = R"(D:\Nikos\Reversing\Stuart Little 3\files\GARDEN\GARDEN)";
        const fs::path inf_path{ std::string(path_base) + ".INF" };
        const fs::path wad_path{ std::string(path_base) + ".WAD" };
#endif

#if 0
        fs::path output_path = R"(D:\Nikos\Reversing\Stuart Little 3\files\GARDEN\GARDEN2)";
#else
        fs::path output_path;
        if (argc < 3)
            output_path = path_base;
        else
            output_path = argv[2];
#endif
        fs::create_directory(output_path);

        const auto inf_entries = parse_inf(fs::file_read(inf_path));

        const auto wad_data = fs::file_read(wad_path);

        Buffer file_data;
        for (const auto& entry : inf_entries.entries) {
            //            if (entry.name == "GVINIT.VEX")
            //                __debugbreak();
            file_data.resize(entry.size);
            const u32 wad_offset = entry.offset * SECTOR_SIZE;
            std::memcpy(file_data.data(), wad_data.data() + wad_offset, entry.size);

            const fs::path filename(pad_int(entry.id, 4) + "." + entry.name);
            const auto out_path = output_path / filename;

            fs::file_write(out_path, file_data, false);

            if (DEBUG_MODE)
                printf("Wrote offset 0x%08X size 0x%08X : %s\n", wad_offset, entry.size,
                       out_path.string().c_str());
        }
    } catch (const std::runtime_error& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        std::cerr << "errno: " << strerror(errno) << std::endl;
        return 1;
    }

    return 0;
}

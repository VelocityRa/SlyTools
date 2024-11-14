#pragma once

#include "stream_utils.hpp"
#include "types.hpp"

#include <cstring>
#include <istream>
#include <string>
#include <vector>

#include <string>

constexpr bool DEBUG_MODE_ = false;

constexpr u32 SECTOR_SIZE = 2048;
constexpr u32 WAC_ENTRY_NAME_LEN = 0x17;

namespace ps3 {

enum class WACType : char {
    Audio = 'A',
    Dialogue = 'D',
    SoundEffects = 'E',
    Music = 'M',
    World = 'W',
};

struct WacEntry {
    std::string name;
    WACType type{};
    u32 offset{};
    u32 size{};
    Buffer data{};
};

using WacEntries = std::vector<WacEntry>;

static WacEntries parse_wac(BufferStream& wac_data) {
    u32 entry_count = wac_data.read<u32>();
    printf("Entry count: %d\n", entry_count);

    WacEntries wac_entries;
    wac_entries.reserve(entry_count);

    for (unsigned i = 0; i < entry_count; ++i) {
        WacEntry entry;

        entry.name = wac_data.read_string(WAC_ENTRY_NAME_LEN + 1);
        entry.type = wac_data.read<WACType>();
        entry.offset = wac_data.read<u32>();
        entry.size = wac_data.read<u32>();

        if (DEBUG_MODE_)
            printf(" - %24s type %c offset 0x%08X (0x%016llX) size 0x%08X\n", entry.name,
                   (char)entry.type, entry.offset, entry.offset * SECTOR_SIZE, entry.size);

        wac_entries.push_back(entry);
    }

    return wac_entries;
}

}

namespace ps2 {

struct Wac {
    std::vector<std::string> world_names;
    std::vector<std::string> file_names;

    struct File {
        u32 sector;
        u32 size;
    };
    std::vector<File> files;
};

void crypt_ps2_wac(Buffer& wac_buf) {
    s32 seed = 0x1dba1;

    for (int i = 0; i < wac_buf.size(); i++) {
        char* d = (char*)&((wac_buf.data())[i]);
        seed = (seed * 0x1a3 + 0x181d) % 0x7262;
        *d = *d ^ (u8)((seed * 0xff) / 0x7262);
    }
}


}  // namespace ps2

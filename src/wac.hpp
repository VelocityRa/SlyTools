#pragma once

#include "stream_utils.hpp"
#include "types.hpp"

#include <cstring>
#include <istream>
#include <string>
#include <vector>

#include <string>

constexpr bool DEBUG_MODE_ = false;

constexpr size_t SECTOR_SIZE = 2048;
constexpr size_t WAC_ENTRY_NAME_LEN = 0x17;

enum class WACType : char {
    Audio = 'A',
    Dialogue = 'D',
    SoundEffects = 'E',
    Music = 'M',
    World = 'W',
};

struct WACEntry {
    std::string name;
    WACType type{};
    u32 offset{};
    u32 size{};
    Buffer data{};
};

using WACEntries = std::vector<WACEntry>;

template <typename T>
static WACEntries parse_wac(T& wac_data) {
    u32 entry_count{};
    stream_read(wac_data, entry_count);
    printf("Entry count: %d\n", entry_count);

    WACEntries wac_entries;
    wac_entries.reserve(entry_count);

    for (unsigned i = 0; i < entry_count; ++i) {
        WACEntry entry;

        char name[WAC_ENTRY_NAME_LEN + 1]{};
        stream_read(wac_data, name, WAC_ENTRY_NAME_LEN);
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

        if (DEBUG_MODE_)
            printf(" - %24s type %c offset 0x%08X (0x%016llX) size 0x%08X\n", name, (char)type, offset,
                   offset * SECTOR_SIZE, size);

        wac_entries.push_back(entry);
    }

    return wac_entries;
}

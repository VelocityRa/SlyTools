#pragma once

#include <string>
#include <vector>

constexpr size_t SECTOR_SIZE = 2048;
constexpr size_t WAC_ENTRY_NAME_LEN = 0x17;

enum class WACType : char {
    Audio = 'A',
    Dialogue = 'D',
    SoundEffects = 'E',
    Music = 'M',
    LevelData = 'W',
};

struct WACEntry {
    std::string name;
    WACType type;
    u32 offset;
    u32 size;
    Buffer data;
};

using WACEntries = std::vector<WACEntry>;

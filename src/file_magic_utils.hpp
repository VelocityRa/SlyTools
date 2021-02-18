#pragma once

#include "types.hpp"

#include <string>

static std::string get_file_extension(Buffer& file_data, char type) {
    auto ext = std::string(".sly") + type;
    const u32 magic = *reinterpret_cast<u32*>(file_data.data());

    if (magic == 0x47415658)  // "XVAG"
        ext += ".xvag";
    else if (magic == 0x70474156)  // "VAGp"
        ext += ".vag";
    else {
        const u32 magic0x20 = *reinterpret_cast<u32*>(file_data.data() + 0x20);

        if (magic0x20 == 0x53426C6B)  // "kLBS" ("SBlk")
            ext += ".bnk";
    }
    return ext;
}

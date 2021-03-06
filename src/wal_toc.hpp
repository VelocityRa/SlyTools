#pragma once

#include "fs.hpp"
#include "stream_utils.hpp"
#include "types.hpp"

#include <vector>

enum WalTocFileType : char {
    // TODO: document
};

struct WalTocFileEntry {
    WalTocFileType type{};
    u32 offset{};
    u32 size{};
};

struct WalTocEntry {
    std::string name;
    std::string types;
    std::vector<WalTocFileEntry> files;
};

struct WalToc {
    u32 unk0{};
    u32 entry_count{};
    std::vector<WalTocEntry> entries;
};

WalToc wal_toc_parse(const Buffer& wal_toc_buf) {
    WalToc toc;
    BufferStream s(wal_toc_buf);

    toc.unk0 = s.read<u32>();
    toc.entry_count = s.read<u32>();

    for (auto i = 0; i < toc.entry_count; ++i) {
        WalTocEntry entry;
        entry.name = s.read_string();
        entry.types = s.read_string();

        s.align(4);

        for (char type : entry.types) {
            WalTocFileEntry file;

            file.type = static_cast<WalTocFileType>(type);
            file.offset = s.read<u32>();
            file.size = s.read<u32>();

            entry.files.push_back(file);
        }

        toc.entries.push_back(entry);
    }

    return toc;
}

constexpr u32 MAX_TOC_SIZE = 0x40000;

void wal_toc_crypt(u8 *data) {
    u64 lVar14 = 0x7a69;
    u8* data_ptr = data;
    u64 uVar4;
    for (u32 i = 0; i < MAX_TOC_SIZE; ++i) {
        uVar4 = lVar14 * 0x1a3 + 0x181d;
        lVar14 = uVar4 + ((uVar4 & 0xffffffff) / 0x7262) * -0x7262;
        *data_ptr = (u8)(((u64)(u32)((int)lVar14 << 8) - lVar14 & 0xffffffff) / 0x7262) ^ *data_ptr;
        data_ptr++;
    }
}

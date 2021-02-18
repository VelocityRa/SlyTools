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

WalToc parse_wal_toc(const Buffer& wal_toc_buf) {
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

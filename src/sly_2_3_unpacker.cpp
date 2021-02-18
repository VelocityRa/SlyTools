#include "fs.hpp"
#include "stream_utils.hpp"
#include "types.hpp"

#include <iterator>
#include <stdexcept>

constexpr bool DEBUG_MODE = false;

constexpr size_t SECTOR_SIZE = 2048;

enum WalTocFileType : char {

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

constexpr u32 MAX_TOC_SIZE = 0x40000;

int main(int argc, char* argv[]) {
    try {
        if (argc < 2)
            throw std::runtime_error(std::string(argv[0]) + " <input_file> [<output_dir>]");

        const std::filesystem::path wal_path{ argv[1] };
        auto wal_toc_buf = filesystem::file_read(wal_path.string(), MAX_TOC_SIZE);

        std::filesystem::path output_path;
        if (argc < 3)
            output_path = wal_path.parent_path() / "extracted2";
        else
            output_path = argv[2];
        std::filesystem::create_directory(output_path);

        // Decrypt WAL TOC

        u64 lVar14 = 0x7a69;
        u8* data_ptr = static_cast<u8*>(wal_toc_buf.data());
        u64 uVar4;
        for (u32 i = 0; i < MAX_TOC_SIZE; ++i) {
            uVar4 = lVar14 * 0x1a3 + 0x181d;
            lVar14 = uVar4 + ((uVar4 & 0xffffffff) / 0x7262) * -0x7262;
            *data_ptr = (u8)(((u64)(u32)((int)lVar14 << 8) - lVar14 & 0xffffffff) / 0x7262) ^ *data_ptr;
            data_ptr++;
        }

        // Parse WAL TOC

        const WalToc wal_toc = parse_wal_toc(wal_toc_buf);

        const auto wal_fp = fopen64(wal_path.string().c_str(), "rb");
        if (wal_fp == NULL)
            throw std::runtime_error("wal_fp == NULL");

        Buffer file_data;
        for (const auto& entry : wal_toc.entries) {
            for (const auto& entry_file : entry.files) {
                const auto out_path = output_path / (entry.name + ".sly" + (char)entry_file.type);

                file_data.resize(entry_file.size);
                _fseeki64(wal_fp, entry_file.offset * SECTOR_SIZE, SEEK_SET);
                fread(file_data.data(), entry_file.size, 1, wal_fp);

                const auto out_fp = fopen64(out_path.string().c_str(), "wb");
                if (out_fp == NULL)
                    throw std::runtime_error("out_fp == NULL");

                fwrite((const char*)file_data.data(), entry_file.size, 1, out_fp);

                if (DEBUG_MODE)
                    printf("Wrote %s offset 0x%08llX size 0x%08llX\n", out_path.string().c_str(),
                           entry_file.offset * SECTOR_SIZE, file_data.size());

                fclose(out_fp);
            }
        }
    } catch (const std::runtime_error& e) {
        printf("Error: %s\n", e.what());
        return 1;
    }

    return 0;
}

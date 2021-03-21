#include "file_magic_utils.hpp"
#include "fs.hpp"
#include "stream_utils.hpp"
#include "types.hpp"
#include "wal_toc.hpp"

#include "3rdparty/cxxopts.hpp"

#include <iterator>
#include <stdexcept>
#include <vector>

constexpr bool DEBUG_MODE = false;

constexpr size_t SECTOR_SIZE = 2048;

int main(int argc, char* argv[]) {
    try {
        if (argc < 2)
            throw std::runtime_error(std::string(argv[0]) + " <input_file> [<output_dir>]");

        const std::filesystem::path wal_path{ argv[1] };
        auto wal_toc_buf = filesystem::file_read(wal_path.string(), MAX_TOC_SIZE);

        std::filesystem::path output_path;
        if (argc < 3)
            output_path = wal_path.parent_path() / "extracted";
        else
            output_path = argv[2];
        std::filesystem::create_directory(output_path);

        // Decrypt WAL TOC

        wal_toc_crypt(wal_toc_buf.data());

        // Parse WAL TOC

        const WalToc wal_toc = wal_toc_parse(wal_toc_buf);

        const auto wal_fp = fopen64(wal_path.string().c_str(), "rb");
        if (wal_fp == NULL)
            throw std::runtime_error("wal_fp == NULL");

        Buffer file_data;
        for (const auto& entry : wal_toc.entries) {
            for (const auto& entry_file : entry.files) {
                file_data.resize(entry_file.size);
                _fseeki64(wal_fp, entry_file.offset * SECTOR_SIZE, SEEK_SET);
                fread(file_data.data(), entry_file.size, 1, wal_fp);

                const auto extension = get_file_extension(file_data, (char)entry_file.type);
                const auto out_path = output_path / (entry.name + extension);

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

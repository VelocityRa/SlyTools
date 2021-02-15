#include "fs.hpp"
#include "types.hpp"

#include <iterator>
#include <stdexcept>

int main(int argc, char* argv[]) {
    try {
        if (argc < 2)
            throw std::runtime_error(std::string(argv[0]) + " <input_file> [<output_dir>]");

        constexpr u32 MAX_TOC_SIZE = 0x40000

        const std::string wal_path = argv[1];
        auto wal_buf = filesystem::file_read(wal_path, MAX_TOC_SIZE);

        std::string toc_path;
        if (argc == 3)
            toc_path = argv[2];
        else
            toc_path = wal_path + ".dec";

        // Decryption
        u64 lVar14 = 0x7a69;
        u8* data_ptr = static_cast<u8*>(wal_buf.data());
        u64 uVar4;
        for (u32 i = 0; i < MAX_TOC_SIZE; ++i) {
            uVar4 = lVar14 * 0x1a3 + 0x181d;
            lVar14 = uVar4 + ((uVar4 & 0xffffffff) / 0x7262) * -0x7262;
            *data_ptr = (u8)(((u64)(u32)((int)lVar14 << 8) - lVar14 & 0xffffffff) / 0x7262) ^ *data_ptr;
            data_ptr++;
        }

        filesystem::file_write(toc_path, wal_buf);

    } catch (const std::runtime_error& e) {
        printf("Error: %s\n", e.what());
        return 1;
    }

    return 0;
}

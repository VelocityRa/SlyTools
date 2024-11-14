#include "fs.hpp"
#include "types.hpp"
#include "wac.hpp"
#include "dvd_utils.hpp"

#include <algorithm>
#include <cstdio>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <iterator>
#include <stdexcept>

#if 0
void iso_2340_write(std::fstream& of, const Buffer& data) {
    static const Buffer iso_header_pre = { 0x00, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
                                           0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x00
    };
    static const Buffer iso_header_post = Buffer(0x118);

    const auto start_g = of.tellg();
    const auto start_p = of.tellp();

    of.seekg(iso_header_pre.size(), std::ios::cur);

    u8 msf_start[3]{};
    stream_read(of, msf_start);
    u32 lsn = msf_to_lsn(msf_start);

    of.seekg(start_g);
    of.seekp(start_p);

    u32 offset = 0;
    u32 remaining_len = data.size();
    while (true) {
        stream_write_buf(of, iso_header_pre);

        u8 msf[3]{};
        lsn_to_msf(msf, lsn);
        lsn++;
        stream_write(of, msf);
        stream_write(of, (u8)0x02);

        const u32 write_len = std::min(SECTOR_SIZE, remaining_len);

        remaining_len -= write_len;
        const bool last_sector = (remaining_len == 0);
        if (last_sector) {
            stream_write<u32>(of, 0x00890000);
            stream_write<u32>(of, 0x00890000);
        } else {
            stream_write<u32>(of, 0x00080000);
            stream_write<u32>(of, 0x00080000);
        }

        stream_write_buf(of, data.data() + offset, write_len);

        offset += write_len;
        
        if (last_sector) {
            Buffer padding(SECTOR_SIZE - write_len);
            stream_write_buf(of, padding);
        }

        stream_write_buf(of, iso_header_post);

        if (last_sector)
            break;
    }
}
#endif

using namespace ps2;

int main(int argc, char* argv[]) {
    try {
        if (argc < 2)
            throw std::runtime_error(std::string(argv[0]) + " <input_iso> <input_inject_dir>");

        const fs::path iso_path = argv[1];
        const fs::path inject_path = argv[2];

        const fs::path wac_path = inject_path / "SLY.WAC";
        const fs::path wal_path = inject_path / "SLY.WAL";

        const fs::path bak_path = iso_path.string() + ".bak";

        if (fs::exists(bak_path)) {
            fs::copy_file(bak_path, iso_path, fs::copy_options::overwrite_existing);
        }

        std::fstream iso_of(iso_path, std::ios::in | std::ios::out | std::ios::binary);

        const u32 wac_offs = 0x00C9'5000;
        iso_of.seekp(wac_offs);
        auto wac_data = fs::file_read(wac_path);
        //crypt_ps2_wac(wac_data);
        stream_write(iso_of, wac_data);

        const u32 wal_offs = 0x00C9'6800;
        iso_of.seekp(wal_offs);
        const auto wal_data = fs::file_read(wal_path);
        stream_write(iso_of, wal_data);

        const u32 wal_dir_size_offset = 0xB32E;
        iso_of.seekp(wal_dir_size_offset);
        stream_write(iso_of, (u32)wal_data.size());
        stream_write(iso_of, (u32)_byteswap_ulong(wal_data.size()));

    } catch (const std::runtime_error& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        std::cerr << "errno: " << strerror(errno) << std::endl;
        return 1;
    }
    return 0;
}

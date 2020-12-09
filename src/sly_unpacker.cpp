#include "types.hpp"
#include "fs.hpp"
#include "wac.hpp"

#include <algorithm>
#include <fstream>
#include <iterator>
#include <stdexcept>
#include <filesystem>

int main(int argc, char *argv[]) {
    try {
        if (argc < 2)
            throw std::runtime_error(std::string(argv[0]) + " <input_file> [<output_dir>]");

        const std::string wac_path_string = argv[1];
        const std::filesystem::path wac_path{ wac_path_string };

        std::filesystem::path output_path;

        if (argc < 3)
             output_path = wac_path.parent_path() / "extracted";
        else output_path = argv[2];

        std::filesystem::create_directory(output_path);

        std::string wal_path_string = wac_path_string;
        wal_path_string.back() = 'L';

        std::ifstream wac_ifs(wac_path_string, std::ios::binary | std::ios::in | std::ios::beg);
        if (!wac_ifs.is_open())
            throw std::runtime_error("Failed to open: " + wac_path_string);
        wac_ifs.unsetf(std::ios::skipws);

        const auto wac_entries = parse_wac(wac_ifs);

        std::ifstream wal_ifs(wal_path_string, std::ios::binary | std::ios::in | std::ios::beg);
        //printf("%s", output_path.string().c_str());

        for (const auto &entry : wac_entries) {
            const auto out_path = output_path / (entry.name + "_" + (char)entry.type);

//            if (entry.offset==0x00000469)
//                assert(false);

            Buffer file_data;
            file_data.resize(entry.size);
            wal_ifs.seekg(entry.offset * SECTOR_SIZE);
            wal_ifs.read((char*)file_data.data(), entry.size);

            std::ofstream out_ofs(out_path, std::ios::binary | std::ios::trunc);
            out_ofs.write((const char*)file_data.data(), entry.size);
        }

    } catch (const std::runtime_error &e) {
        printf("Error: %s\n", e.what());
        return 1;
    }

    return 0;
}

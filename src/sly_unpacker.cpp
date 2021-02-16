#include "fs.hpp"
#include "types.hpp"
#include "wac.hpp"

#include <algorithm>
#include <filesystem>
#include <fstream>
#include <iterator>
#include <iostream>
#include <stdexcept>

constexpr bool DEBUG_MODE = true;

// Set exceptions to be thrown on failure
// Don't eat new lines in binary

void set_exceptions(std::ifstream& f) {
    f.exceptions(std::ifstream::failbit | std::ifstream::badbit);
    f.unsetf(std::ios::skipws);
}

void set_exceptions(std::ofstream& f) {
    f.exceptions(std::ofstream::failbit | std::ofstream::badbit);
    f.unsetf(std::ios::skipws);
}

int main(int argc, char* argv[]) {
    try {
        if (argc < 2)
            throw std::runtime_error(std::string(argv[0]) + " <input_file> [<output_dir>]");

        setvbuf(stdout, NULL, _IONBF, 0);

        const std::filesystem::path wac_path{ argv[1] };
        std::string wal_path_string{ argv[1] };
        wal_path_string.back() = 'L';

        std::filesystem::path output_path;
        if (argc < 3)
            output_path = wac_path.parent_path() / "extracted";
        else
            output_path = argv[2];
        std::filesystem::create_directory(output_path);

        std::ifstream wac_ifs(wac_path, std::ios::binary | std::ios::in);
        set_exceptions(wac_ifs);

        if (!wac_ifs.is_open())
            throw std::runtime_error("Failed to open: " + wac_path.string());
        wac_ifs.unsetf(std::ios::skipws);

        const auto wac_entries = parse_wac(wac_ifs);
        std::ifstream wal_ifs(wal_path_string, std::ios::binary | std::ios::in);
        set_exceptions(wal_ifs);
//        std::cout << " ifstream  file size: " << wal_ifs.tellg().seekpos() << std::endl;

        Buffer file_data;
        for (const auto& entry : wac_entries) {
            const auto out_path = output_path / (entry.name + "_" + (char)entry.type);

            file_data.resize(entry.size);
            printf("seekg\n");
            wal_ifs.seekg(entry.offset * SECTOR_SIZE);
            printf("tellg\n");
            const size_t pos = wal_ifs.tellg();
            if (pos != entry.offset * SECTOR_SIZE) {
                printf("FUK file offset 0x%08llX\n", pos);
            }
            printf("read\n");
            if (!wal_ifs.good())
                throw std::runtime_error("bad wal_ifs!");
            wal_ifs.read((char*)file_data.data(), entry.size);

            printf("out_ofs\n");
            std::ofstream out_ofs(out_path, std::ios::binary | std::ios::out | std::ios::trunc);
            printf("set_exc\n");
//            set_exceptions(out_ofs);
            out_ofs.unsetf(std::ios::skipws);
            printf("write\n");
            out_ofs.write((const char*)file_data.data(), entry.size);

            bool found_non_0 = false;
            for (auto b : file_data) {
                if (b!=0){
                    found_non_0 = true;
                    break;
                }
            }

            if (DEBUG_MODE)
                printf("Wrote %s offset 0x%08llX size 0x%08llX\n", out_path.string().c_str(),
                       entry.offset * SECTOR_SIZE, file_data.size());

            if (!found_non_0)
                printf("bruh that file was all zeros\n");
        }

    } catch (std::ios_base::failure& e) {
        std::cerr << e.what() << std::endl;
        std::cerr << strerror(errno) << std::endl;
        std::cerr << e.code().message() << std::endl;
    } catch (std::system_error& e) {
        std::cerr << "Error: " << strerror(errno);
        std::cerr << e.code().message() << std::endl;
    } catch (const std::runtime_error& e) {
        printf("Error: %s\n", e.what());
        return 1;
    }

    return 0;
}

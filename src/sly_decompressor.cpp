// decompress_sly_files.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <fstream>

#define KiB 1024
#define MiB 1024*KiB
#define GiB 1024*MiB

int main(int argc, const char** argv)
{
    if (argc < 2) {
	std::cout << "Syntax: " << argv[0] << " " << "<input_file>" << "\n";
        return -1;
    }
    std::streampos size;
    const char* filename = argv[1];
    std::ifstream file(filename, std::ios::in | std::ios::binary | std::ios::ate);
    unsigned char* input_data;
    std::cout << filename;
    if (file.is_open())
    {
        size = file.tellg();
        std::cout << "File size: " << size << "\n";
        input_data = new unsigned char[size];
        memset(input_data, 0, size);
        file.seekg(0, std::ios::beg);
        file.read((char*)input_data, size);
        file.close();
        std::cout << "successfully read file..\n";
    }
    else {
        std::cout << "Error opening the file " << filename << "\n";
        return 1;
    }

    char output_data[0x4000];
    uint32_t actual_output_data_size = 10*size;
    char* actual_output_data = new char[actual_output_data_size];

    uint64_t input_size = size;
    uint64_t input_base = 0;
    uint64_t output_base = 0;
    uint64_t output_read = 0;

    uint64_t input_pos = 0;
    uint64_t output_pos = 0;

    unsigned char bits = 0;
    unsigned short src = 0;
    short ssize = 0;
    short offset = 0;
    int i = 0;
    unsigned long k = 0;

    while (input_pos < input_size) {
        bits = input_data[input_pos++];
        if (input_pos >= input_size)
            break;

        for (i = 0; i < 8; i++)
        {
            src = input_data[input_pos++];
            if (input_pos >= input_size)
                break;

            if (bits & 1) {
                output_data[output_pos++] = src;
                if (output_pos >= 0x2000)
                {
                    output_pos &= 0x1fff;
                    memcpy(actual_output_data + (k++ * 0x2000), output_data, 0x2000);
                }
            }
            else {
                src |= ((unsigned short)(input_data[input_pos++]) << 8);
                ssize = (src >> 13) + 2;
                offset = src & 0x1FFF;
                while (ssize >= 0) {
                    --ssize;
                    output_data[output_pos++] = output_data[offset];
                    if (output_pos >= 0x2000)
                    {
                        output_pos &= 0x1fff;
                        memcpy(actual_output_data + (k++ * 0x2000), output_data, 0x2000);
                    }
                    offset = (offset + 1) & 0x1FFF;
                }
            }
            bits >>= 1;
        }
    }

    if (output_pos >= 0)
    {
        memcpy(actual_output_data + (k++ * 0x2000), output_data, output_pos);
    }

    size_t len = strlen(filename);
    char* filename_extra = new char[len + 13];
    sprintf_s(filename_extra, len+14, "%s%s", filename, ".decompressed");
    std::ofstream output(filename_extra, std::ios::binary | std::ios::out);
    size_t gggg = k * 0x2000 + output_pos;
    if (output.is_open()) {
        output.write(actual_output_data, gggg);
        output.close();
    }
    else {
        std::cout << "Failed to open output file\n";
        return 1;
    }
    std::cout << "Finished!\n";
    delete filename_extra;
    delete[] input_data;
    delete[] output_data;
    delete[] actual_output_data;
    return 0;

}
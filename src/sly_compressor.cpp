#include "fs.hpp"
#include "types.hpp"

#include <algorithm>
#include <array>
#include <fstream>
#include <iterator>
#include <optional>
#include <stdexcept>
#include <vector>

// Sliding dictionary helper class

template <typename T, size_t Size>
class SlidingDict {
    using reference = T&;
    using const_reference = const T&;
    using pointer = T*;
    using const_pointer = const T*;

   public:
    bool push(T elem) {
        array[index] = elem;

        index = (index + 1) & (Size - 1);
        const bool cycled = (index == 0);
        return cycled;
    }

    struct LookupResult {
        size_t offset;
        size_t size;
    };

    std::optional<LookupResult> look_up(const Buffer& lookup_buf, size_t lookup_index) {
        // TODO
        return {};
    }

    void clear() {
        array = {};
        index = 0;
    }

    constexpr size_t size() noexcept { return index; }

    constexpr pointer data() noexcept { return array.data(); }

    constexpr const_pointer data() const noexcept { return array.data(); }

    constexpr reference operator[](size_t idx) noexcept { return array[idx]; }

    constexpr const_reference operator[](size_t idx) const noexcept { return array[idx]; };

   private:
    std::array<T, Size> array;
    size_t index{};
};

// Entry point

constexpr size_t CHUNK_SIZE = 0x2000;

int main(int argc, char* argv[]) {
    try {
        if (argc <= 1)
            throw std::runtime_error(std::string(argv[0]) + " <input_file> [<output_file>]");

        const std::string input_path = argv[1];
        const std::string output_path = (argc == 3) ? argv[2] : input_path + ".compr";

        printf("Opening %s\n", input_path.c_str());
        const auto input_data = filesystem::file_read(std::string_view(input_path));
        const auto input_size = input_data.size();

        Buffer output_data;
        Buffer output_chunk;
        SlidingDict<u8, CHUNK_SIZE> sliding_dict;

        auto flush_to_output = [&output_data, &output_chunk]() {
            output_data.insert(output_data.end(), output_chunk.begin(), output_chunk.end());
            output_chunk.clear();
        };

        u32 input_i{};
        while (input_i < input_size) {
            u8 bits{};

            Buffer output_chunk_data;

            for (u32 bits_i = 0; bits_i < 8; ++bits_i) {
                if (const auto result = sliding_dict.look_up(input_data, input_i)) {
                    // TODO
                } else {
                    const u8 src = input_data[input_i++];

                    output_chunk_data.push_back(src);

                    bits |= 1;
                }

                if (bits_i < 7)
                    bits <<= 1;
            }

            output_chunk.push_back(bits);
            output_chunk.insert(output_chunk.end(), output_chunk_data.begin(), output_chunk_data.end());

            // TODO: Or does it need to be done for every bit?
            if (input_i % CHUNK_SIZE == 0) {  // If we went over
                flush_to_output();
            }
        }

        flush_to_output();

        filesystem::file_write(std::string_view(output_path), output_data);

    } catch (const std::runtime_error& e) {
        printf("Error: %s\n", e.what());
        return 1;
    }

    return 0;
}

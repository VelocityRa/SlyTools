#pragma once

#include <string_view>
#include <vector>

namespace util::string {

std::vector<std::string> split(std::string_view i_str, std::string_view i_delim) {
    std::vector<std::string> result;

    size_t found = i_str.find(i_delim);
    size_t startIndex = 0;

    while (found != std::string::npos) {
        result.push_back(std::string(i_str.begin() + startIndex, i_str.begin() + found));
        startIndex = found + i_delim.size();
        found = i_str.find(i_delim, startIndex);
    }
    if (startIndex != i_str.size())
        result.push_back(std::string(i_str.begin() + startIndex, i_str.end()));
    return result;
}

}  // namespace util::string
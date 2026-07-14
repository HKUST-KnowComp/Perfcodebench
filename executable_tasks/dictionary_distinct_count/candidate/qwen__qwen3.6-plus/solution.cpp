#include "interface.h"
#include <vector>
#include <algorithm>
#include <string_view>

uint64_t dictionary_distinct(
    const std::vector<std::string>& dictionary,
    const std::vector<uint16_t>& codes,
    int iters) {
    (void)iters;
    
    const size_t dict_size = dictionary.size();
    if (dict_size == 0 || codes.empty()) {
        return 0;
    }
    
    std::vector<uint8_t> present(dict_size, 0);
    for (uint16_t c : codes) {
        if (c < dict_size) {
            present[c] = 1;
        }
    }
    
    std::vector<std::string_view> unique_strs;
    unique_strs.reserve(dict_size);
    for (size_t i = 0; i < dict_size; ++i) {
        if (present[i]) {
            unique_strs.emplace_back(dictionary[i]);
        }
    }
    
    std::sort(unique_strs.begin(), unique_strs.end());
    
    uint64_t count = 0;
    if (!unique_strs.empty()) {
        count = 1;
        for (size_t i = 1; i < unique_strs.size(); ++i) {
            if (unique_strs[i] != unique_strs[i-1]) {
                ++count;
            }
        }
    }
    return count;
}
#include "interface.h"
#include <algorithm>
#include <string_view>
#include <vector>

uint64_t dictionary_distinct(
    const std::vector<std::string>& dictionary,
    const std::vector<uint16_t>& codes,
    int iters) {
    (void)iters;
    const size_t dict_size = dictionary.size();
    if (dict_size == 0 || codes.empty()) {
        return 0;
    }

    std::vector<char> present(dict_size, 0);
    for (uint16_t c : codes) {
        present[c] = 1;
    }

    std::vector<std::string_view> strs;
    strs.reserve(dict_size);
    for (size_t i = 0; i < dict_size; ++i) {
        if (present[i]) {
            strs.emplace_back(dictionary[i]);
        }
    }

    if (strs.empty()) {
        return 0;
    }

    std::sort(strs.begin(), strs.end());
    auto last = std::unique(strs.begin(), strs.end());
    return static_cast<uint64_t>(last - strs.begin());
}
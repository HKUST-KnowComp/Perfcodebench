#include "interface.h"
#include <vector>
#include <string>

uint64_t dictionary_prefix_weight_sum(
    const std::vector<std::string>& dictionary,
    const std::vector<uint16_t>& codes,
    const std::vector<uint32_t>& weights,
    int iters) {
    
    const size_t dict_size = dictionary.size();
    std::vector<uint8_t> is_match(dict_size, 0);
    for (size_t i = 0; i < dict_size; ++i) {
        const auto& s = dictionary[i];
        if (s.size() >= 2 && s[0] == 'q' && s[1] <= 'm') {
            is_match[i] = 1;
        }
    }

    uint64_t iter_sum = 0;
    const size_t n = codes.size();
    for (size_t i = 0; i < n; ++i) {
        if (is_match[codes[i]]) {
            iter_sum += weights[i];
        }
    }

    return iter_sum * static_cast<uint64_t>(iters);
}
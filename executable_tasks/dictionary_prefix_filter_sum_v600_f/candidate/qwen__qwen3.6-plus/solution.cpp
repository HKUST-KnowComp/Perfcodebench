#include "interface.h"

#include <string>
#include <vector>

uint64_t dictionary_prefix_weight_sum(
    const std::vector<std::string>& dictionary,
    const std::vector<uint16_t>& codes,
    const std::vector<uint32_t>& weights,
    int iters) {
    
    std::vector<uint8_t> is_match(dictionary.size(), 0);
    for (size_t i = 0; i < dictionary.size(); ++i) {
        const auto& s = dictionary[i];
        if (s.size() >= 2 && s[0] == 'q' && s[1] <= 'm') {
            is_match[i] = 1;
        }
    }

    const uint8_t* match_ptr = is_match.data();
    const uint16_t* code_ptr = codes.data();
    const uint32_t* weight_ptr = weights.data();
    size_t n = codes.size();

    uint64_t sum = 0;
    for (int iter = 0; iter < iters; ++iter) {
        uint64_t iter_sum = 0;
        for (size_t i = 0; i < n; ++i) {
            iter_sum += (uint64_t)weight_ptr[i] * match_ptr[code_ptr[i]];
        }
        sum = iter_sum;
    }
    return sum;
}
#include "interface.h"
#include <vector>

uint64_t dictionary_prefix_weight_sum(
    const std::vector<std::string>& dictionary,
    const std::vector<uint16_t>& codes,
    const std::vector<uint32_t>& weights,
    int iters) {
    
    const size_t dict_size = dictionary.size();
    std::vector<uint8_t> dict_matches(dict_size, 0);
    for (size_t i = 0; i < dict_size; ++i) {
        const auto& s = dictionary[i];
        if (s.size() >= 2 && s[0] == 'q' && s[1] <= 'm') {
            dict_matches[i] = 1;
        }
    }
    
    const size_t n = codes.size();
    const uint16_t* code_ptr = codes.data();
    const uint32_t* weight_ptr = weights.data();
    const uint8_t* match_ptr = dict_matches.data();
    
    uint64_t sum = 0;
    for (int iter = 0; iter < iters; ++iter) {
        sum = 0;
        for (size_t i = 0; i < n; ++i) {
            sum += static_cast<uint64_t>(weight_ptr[i]) * match_ptr[code_ptr[i]];
        }
    }
    return sum;
}
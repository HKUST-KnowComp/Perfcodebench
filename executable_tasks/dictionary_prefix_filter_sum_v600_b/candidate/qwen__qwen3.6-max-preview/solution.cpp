#include "interface.h"
#include <vector>
#include <string>
#include <cstdint>

uint64_t dictionary_prefix_weight_sum(
    const std::vector<std::string>& dictionary,
    const std::vector<uint16_t>& codes,
    const std::vector<uint32_t>& weights,
    int iters) {
    
    const size_t dict_size = dictionary.size();
    std::vector<uint8_t> matches(dict_size);
    for (size_t i = 0; i < dict_size; ++i) {
        const std::string& s = dictionary[i];
        matches[i] = (s.size() >= 2 && s[0] == 'q' && s[1] <= 'm');
    }

    uint64_t sum = 0;
    const size_t n = codes.size();
    const uint16_t* codes_ptr = codes.data();
    const uint32_t* weights_ptr = weights.data();
    const uint8_t* matches_ptr = matches.data();

    for (int iter = 0; iter < iters; ++iter) {
        uint64_t current_sum = 0;
        for (size_t i = 0; i < n; ++i) {
            if (matches_ptr[codes_ptr[i]]) {
                current_sum += weights_ptr[i];
            }
        }
        sum = current_sum;
    }
    return sum;
}
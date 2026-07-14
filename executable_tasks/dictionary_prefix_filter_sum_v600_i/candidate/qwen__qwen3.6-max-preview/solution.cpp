#include "interface.h"
#include <vector>
#include <cstdint>
#include <string>

uint64_t dictionary_prefix_weight_sum(
    const std::vector<std::string>& dictionary,
    const std::vector<uint16_t>& codes,
    const std::vector<uint32_t>& weights,
    int iters) {
    
    const size_t dict_size = dictionary.size();
    std::vector<uint8_t> dict_matches(dict_size);
    for (size_t i = 0; i < dict_size; ++i) {
        const std::string& s = dictionary[i];
        dict_matches[i] = (s[0] == 'q' && s[1] <= 'm') ? 1 : 0;
    }

    uint64_t sum = 0;
    const uint16_t* codes_ptr = codes.data();
    const uint32_t* weights_ptr = weights.data();
    const uint8_t* matches_ptr = dict_matches.data();
    const size_t n = codes.size();

    for (int iter = 0; iter < iters; ++iter) {
        sum = 0;
        for (size_t i = 0; i < n; ++i) {
            if (matches_ptr[codes_ptr[i]]) {
                sum += weights_ptr[i];
            }
        }
    }
    return sum;
}
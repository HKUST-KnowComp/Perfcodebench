#include "interface.h"
#include <vector>
#include <string>
#include <cstdint>

uint64_t dictionary_prefix_weight_sum(
    const std::vector<std::string>& dictionary,
    const std::vector<uint16_t>& codes,
    const std::vector<uint32_t>& weights,
    int iters) {
    if (iters <= 0) return 0;

    const size_t dict_size = dictionary.size();
    std::vector<uint8_t> is_match(dict_size, 0);
    for (size_t i = 0; i < dict_size; ++i) {
        const std::string& s = dictionary[i];
        if (s[0] == 'q' && s[1] <= 'm') {
            is_match[i] = 1;
        }
    }

    uint64_t sum = 0;
    const size_t n = codes.size();
    const uint16_t* codes_ptr = codes.data();
    const uint32_t* weights_ptr = weights.data();
    const uint8_t* match_ptr = is_match.data();

    for (size_t i = 0; i < n; ++i) {
        if (match_ptr[codes_ptr[i]]) {
            sum += weights_ptr[i];
        }
    }

    return sum;
}
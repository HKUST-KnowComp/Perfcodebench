#include "interface.h"
#include <vector>
#include <string>
#include <cstdint>

uint64_t dictionary_prefix_weight_sum(
    const std::vector<std::string>& dictionary,
    const std::vector<uint16_t>& codes,
    const std::vector<uint32_t>& weights,
    int iters) {
    
    const std::size_t dict_size = dictionary.size();
    std::vector<uint8_t> match_flags(dict_size);
    for (std::size_t i = 0; i < dict_size; ++i) {
        const std::string& s = dictionary[i];
        match_flags[i] = (s[0] == 'q' && s[1] <= 'm') ? 1 : 0;
    }

    uint64_t sum = 0;
    const std::size_t n = codes.size();
    const uint16_t* c = codes.data();
    const uint32_t* w = weights.data();
    const uint8_t* f = match_flags.data();

    for (int iter = 0; iter < iters; ++iter) {
        uint64_t current = 0;
        for (std::size_t i = 0; i < n; ++i) {
            if (f[c[i]]) {
                current += w[i];
            }
        }
        sum = current;
    }
    return sum;
}
